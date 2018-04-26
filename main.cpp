#include <cmath>
#include <random>
#include <iostream>
#include <chrono>

#include <omp.h>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

#include "opengl/OpenGLHelper.h"
#include "opengl/CameraHelper.h"
#include "opengl/vao/VoxelGrid.h"

#include "Box3D.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNM
#include "stb_image.h"
#include "Util.h"

#include "pnmio.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

struct Depth {
	int width;
	int height;
    std::vector<float> data;
};

class World {
public:

    void term() {
        
    }
	
	void init() {
		load_depth_map();
		std::string filename_camera = std::string(ASSETS_DIR) + "/camera.yaml";
		load_extrinsics_intrinsics(filename_camera, intr, extr);
		
		init_mvp();
		std::string shader_dir = std::string(HOME_DIR) + "/opengl/shader/";
		create_point_cloud();
		ct::geometry::Box3D::create(vertices, normals);
		ct::vao::VoxelGrid::init_vao(vao, vertices, normals, positions, colors, shader_dir);
	}

	void init_mvp() {
		model_matrix.setIdentity();
		float ar = (float)(WINDOW_WIDTH)/WINDOW_HEIGHT;
		Eigen::Vector3f eye(0, 0, -1);
		Eigen::Vector3f up(0, 1, 0);
		Eigen::Vector3f lookat(0, 0, 0);
		view_matrix = oglh::make_view_matrix<Eigen::Vector3f>(eye, lookat, up);
		projection_matrix = oglh::make_perspective_matrix<float>(60.0f, ar, 0.3f, 50.0f);
	}

	Eigen::Matrix<float, 4, 4> vision2graphics(Eigen::Matrix4f K, int width, int height, float znear, float zfar) {
		Eigen::Matrix4f P;
		P << 
			2*K(0, 0)/width,	0,   		   		 2*(K(0, 2)/width - 1),          	0,
			0, 					2*K(1, 1)/height,	-2*(K(1, 2)/height + 1),      		0,
			0, 					0, 					-(zfar + znear)/(zfar - znear),		-2*zfar*znear/(zfar - znear),
			0, 					0,                  -1, 								0;

		return P;
	}

    void create_point_cloud() {
		//projection_matrix = vision2graphics(intr, depth.width, depth.height, 0.1f, 100.0f);
		Eigen::Matrix4f Kinv = intr.inverse();
		std::cout << "intrinsic-matrix:\n" << intr << std::endl;

       	 
		Eigen::MatrixXf positions0(3, depth.height*depth.width);
		Eigen::MatrixXf colors0(3, depth.height*depth.width);

		int counter = 0;
		for (int i = 0; i < depth.height; i++) {
			for (int j = 0; j < depth.width; j++) {
				float d = depth.data[i*depth.width + j];
				if (d < 0.2 || d > 8.0)
					continue;
				Eigen::Vector4f p = Kinv*Eigen::Vector4f(j*d, i*d, d, 1.0f);		
				const int index = i*depth.width + j;
				positions0.col(index) = p.topRows(3);
				colors0.col(index) = Eigen::Vector3f(0.2, 0.2, 0.2);
				counter++;
			}
		}
		positions = positions0.leftCols(counter);
		colors = colors0.leftCols(counter);
		printf("n_voxels: %d\n", (int)positions.cols());
    }

	void load_depth_map() {

		std::string filename = std::string(ASSETS_DIR) + "/depth0.pnm";        
		int row = 0, col = 0;
		std::ifstream file(filename.c_str(), std::ios::binary);
		std::string s;

		getline(file, s); //version p5
		getline(file, s); // Width and Height

		unsigned int width, height;
		std::stringstream wh(s);
		wh >> width;
		wh >> height;
		

		getline(file, s); //The maximum gray value : 65536

		unsigned char lo;
		unsigned char hi;

		std::vector<uint16_t> data(height*width);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				file.read(reinterpret_cast<char*>(&hi), 1);
				file.read(reinterpret_cast<char*>(&lo), 1);
				int index = i*width + j;
				data[index] = ((unsigned short)(hi << 8) | lo) & 0x1FFF;
				//std::cout << (*this)(j, i) << " ";
			}
		}
		file.close();

		depth.width = width;
		depth.height = height;
		depth.data.resize(width*height);
		std::transform(data.begin(), data.end(), depth.data.data(), [](uint16_t a) -> float {return a/1000.0;});
	}

	void draw() {
		ct::vao::VoxelGrid::draw(vao, positions.cols(), 0.005, model_matrix, view_matrix, projection_matrix);
	}

	void advance(std::size_t iteration_counter, int64_t ms_per_frame) {
		Eigen::Map<Eigen::Vector3f> eye(Camera::cam_position);
		Eigen::Map<Eigen::Vector3f> up(Camera::cam_up);
		Eigen::Map<Eigen::Vector3f> lookat(Camera::cam_lookat);
		view_matrix = oglh::make_view_matrix<Eigen::Vector3f>(eye, lookat, up);	
    }

private:
	Eigen::Matrix4f model_matrix;
	Eigen::Matrix4f projection_matrix;
	Eigen::Matrix4f view_matrix;

	Eigen::Matrix4f extr;
	Eigen::Matrix4f intr;

	Eigen::MatrixXf positions, colors, vertices, normals;
	ct::vao::VoxelGrid::VAO vao;
	Depth depth;
};

int main() {
	GLFWwindow *window;
	oglh::init_gl("example", WINDOW_WIDTH, WINDOW_HEIGHT, &window);
	glfwSetCursorPosCallback(window, Camera::mousemove_glfwCursorPosCallback);
	glfwSetScrollCallback(window, Camera::mousemove_glfwScrollCallback);
	glfwSetMouseButtonCallback(window, Camera::mousemove_glfwMouseButtonCallback);
    World world;
	world.init();

    std::chrono::high_resolution_clock clock;
    std::size_t iteration_counter = 0;
    std::chrono::high_resolution_clock::time_point t0 = clock.now();
    std::chrono::milliseconds mspf;
    while (oglh::is_window_alive(window)) {
        oglh::poll_events();
		oglh::clear_screen();

        world.advance(iteration_counter++, mspf.count());
        world.draw();

        mspf = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - t0);
        t0 = clock.now();

        oglh::swap_window_buffer(window);
    }
	world.term();
    oglh::terminate_window();

    return 0;
}

