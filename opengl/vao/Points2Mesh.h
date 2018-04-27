#pragma once

#include <GL/glew.h>
#include <eigen3/Eigen/Dense>

namespace ct {
	namespace vao {
		namespace Points2Mesh {

			struct VAO {
				GLuint id_program;
				GLuint id_vao, id_vbo_vertices, ebo;

				GLint loc_model_matrix, loc_view_matrix, loc_projection_matrix;
			};

			static void init_vao(VAO &vao, Eigen::MatrixXf &vertices, Eigen::Matrix<uint32_t, -1, -1> &elements, std::string shader_dir) {
				assert(vertices.rows() == 3);

				GLuint vs = 0, fs = 0, gs = 0;
				oglh::load_shader(vs, shader_dir + "/Points2MeshVS.glsl", GL_VERTEX_SHADER);
				oglh::load_shader(gs, shader_dir + "/Points2MeshGS.glsl", GL_GEOMETRY_SHADER);
				oglh::load_shader(fs, shader_dir + "/Points2MeshFS.glsl", GL_FRAGMENT_SHADER);
				GLuint shaders[3] = {vs, gs, fs};
				oglh::create_program(vao.id_program, shaders, 3);
				glUseProgram(vao.id_program);
				glGenVertexArrays(1, &vao.id_vao);
				glBindVertexArray(vao.id_vao);

				vao.loc_model_matrix = glGetUniformLocation(vao.id_program, "model_matrix");
				assert(vao.loc_model_matrix != -1);
				vao.loc_view_matrix = glGetUniformLocation(vao.id_program, "view_matrix");
				assert(vao.loc_view_matrix != -1);
				vao.loc_projection_matrix = glGetUniformLocation(vao.id_program, "projection_matrix");
				assert(vao.loc_projection_matrix != -1);

				// -> vertices (vbo)
				glGenBuffers(1, &vao.id_vbo_vertices);
				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertices);
				glBufferData(GL_ARRAY_BUFFER, 3*vertices.cols()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);
				// <-

				// -> ebo
				glGenBuffers(1, &vao.ebo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*elements.cols()*sizeof(GLuint), elements.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				// <-

				glEnable(GL_DEPTH_TEST);
			}

			static void draw(VAO& vao, int n_size, float scale, Eigen::Matrix4f &model_matrix, Eigen::Matrix4f &view_matrix, Eigen::Matrix4f &projection_matrix) {
				glUseProgram(vao.id_program);
				glBindVertexArray(vao.id_vao);

				glBindBuffer(GL_ARRAY_BUFFER, vao.id_vbo_vertices);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glUniformMatrix4fv(vao.loc_model_matrix, 1, GL_FALSE, model_matrix.data());
				glUniformMatrix4fv(vao.loc_view_matrix, 1, GL_FALSE, view_matrix.data());
				glUniformMatrix4fv(vao.loc_projection_matrix, 1, GL_FALSE, projection_matrix.data());

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.ebo);
				glDrawElements(GL_TRIANGLES, 3*n_size, GL_UNSIGNED_INT, 0);
				//glDrawArrays(GL_POINTS, 0, n_size);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glDisableVertexAttribArray(0);
			}

		}

	}
}
