#include <cmath>
#include <random>
#include <iostream>
#include <chrono>

#include <omp.h>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

#include "opengl/OpenGLHelper.h"
#include "opengl/CameraHelper.h"
#include "opengl/vao/Points2Mesh.h"

#include "opencv2/imgproc/imgproc.hpp"

#include "Util.h"

#define WINDOW_WIDTH (224*4)
#define WINDOW_HEIGHT (172*4)

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
		ct::vao::Points2Mesh::init_vao(vao, positions, elements, shader_dir);
	}

	void init_mvp() {
		model_matrix.setIdentity();
		float ar = (float)(WINDOW_WIDTH)/WINDOW_HEIGHT;
		Eigen::Vector3f eye(0, 0, 0);
		Eigen::Vector3f up(0, -1, 0);
		Eigen::Vector3f lookat(0, 0, 1);
		view_matrix = oglh::make_view_matrix<Eigen::Vector3f>(eye, lookat, up);
		projection_matrix = oglh::make_perspective_matrix<float>(60.0f, ar, 0.1f, 20.0f);
		//projection_matrix = vision2graphics(intr, depth.width, depth.height, 0.1, 50.0);
		std::cout << "intrinsic-matrix:\n" << intr << std::endl;
		std::cout << "projection_matrix:\n" << projection_matrix << std::endl;
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

       	 
		positions.resize(3, depth.height*depth.width);

		for (int i = 0; i < depth.height; i++) {
			for (int j = 0; j < depth.width; j++) {
				float d = depth.data[i*depth.width + j];
				Eigen::Vector4f p = Kinv*Eigen::Vector4f(j*d, i*d, d, 1.0f);		
				const int index = i*depth.width + j;
				positions.col(index) = p.topRows(3);
			}
		}
		elements.resize(3, (depth.height - 1)*(depth.width - 1)*2);
		auto findex = [&](int y, int x) {return y*depth.width + x;};
		for (int i = 0; i < depth.height-1; i++) {
			for (int j = 0; j < depth.width-1; j++) {
				int index = i*(depth.width - 1) + j;
				elements.col(index*2 + 0) = Eigen::Vector3i(findex(i, j), findex(i, j + 1 ), findex(i + 1, j)).cast<uint32_t>();
				elements.col(index*2 + 1) = Eigen::Vector3i(findex(i, j + 1), findex(i + 1, j + 1 ), findex(i + 1, j)).cast<uint32_t>();
			}
		}
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
		cv::Mat tmp(height, width, CV_32FC1, depth.data.data());
		cv::medianBlur(tmp, tmp, 3);
		printf("depthmap: width: %d height: %d\n", width, height);
	}

	void draw() {
		ct::vao::Points2Mesh::draw(vao, elements.cols(), 0.005, model_matrix, view_matrix, projection_matrix);
	}

	void advance(std::size_t iteration_counter, int64_t ms_per_frame) {
		//Eigen::Map<Eigen::Vector3f> eye(Camera::cam_position);
		//Eigen::Map<Eigen::Vector3f> up(Camera::cam_up);
		//Eigen::Map<Eigen::Vector3f> lookat(Camera::cam_lookat);
		//view_matrix = oglh::make_view_matrix<Eigen::Vector3f>(eye, lookat, up);	
    }

private:
	Eigen::Matrix4f model_matrix;
	Eigen::Matrix4f projection_matrix;
	Eigen::Matrix4f view_matrix;

	Eigen::Matrix4f extr;
	Eigen::Matrix4f intr;

	Eigen::MatrixXf positions;
	Eigen::Matrix<uint32_t, -1, -1> elements;
	ct::vao::Points2Mesh::VAO vao;
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

