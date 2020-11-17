#ifndef MAIN_H
#define MAIN_H


#include "uv_camera.h"
#include "primitives.h"
#include "mesh.h"
#include "vertex_fragment_shader.h"
#include "vertex_geometry_fragment_shader.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
using namespace glm;

#include <cstdlib>
#include <GL/glew.h>
#include <GL/glut.h>
#pragma comment(lib, "glew32")



#include <vector>
using std::vector;

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;
using std::istringstream;


void idle_func(void);
bool init_opengl(const int &width, const int &height);
void reshape_func(int width, int height);
void display_func(void);
void keyboard_func(unsigned char key, int x, int y);
void mouse_func(int button, int state, int x, int y);
void motion_func(int x, int y);
void passive_motion_func(int x, int y);


void draw_mesh(void);
void draw_dot(void);


vector<triangle> triangles;
vector<vec3> face_normals;
vector<vec3> vertices;
vector<vec3> vertex_normals;

vertex_geometry_fragment_shader render;
vertex_fragment_shader flat;
vertex_fragment_shader ssao;

uv_camera main_camera;

GLint win_id = 0;
GLint win_x = 800, win_y = 450;

float u_spacer = 0.01f;
float v_spacer = 0.5f*u_spacer;
float w_spacer = 0.1f;

bool lmb_down = false;
bool mmb_down = false;
bool rmb_down = false;
int mouse_x = 0;
int mouse_y = 0;

vec3 ray;


GLuint      render_fbo = 0;
GLuint      fbo_textures[3] = { 0, 0, 0 };
GLuint      quad_vao = 0;
GLuint      points_buffer = 0;




struct
{
	struct
	{
		GLint           mv_matrix;
		GLint           proj_matrix;
		GLint           shading_level;
	} render;


	struct
	{
		GLint           ssao_level;
		GLint           object_level;
		GLint           ssao_radius;
		GLint           weight_by_angle;
		GLint           randomize_points;
		GLint           point_count;
	} ssao;

	struct
	{
		GLint           mv_matrix;
		GLint           proj_matrix;
		GLint			flat_colour;
	} flat;

} uniforms;

bool  show_shading;
bool  show_ao;
float ssao_level;
float ssao_radius;
bool  weight_by_angle;
bool randomize_points;
unsigned int point_count;



struct SAMPLE_POINTS
{
	vec4 point[256];
	vec4 random_vectors[256];
};


static unsigned int seed = 0x13371337;

static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int*)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}



void draw_axis(void)
{
	unsigned int axis_buffer = 0;
	glGenBuffers(1, &axis_buffer);

	glLineWidth(2.0);

	glUseProgram(flat.get_program());

	glUniformMatrix4fv(uniforms.flat.proj_matrix, 1, GL_FALSE, &main_camera.projection_mat[0][0]);
	glUniformMatrix4fv(uniforms.flat.mv_matrix, 1, GL_FALSE, &main_camera.view_mat[0][0]);

	const GLuint components_per_vertex = 3;
	const GLuint components_per_position = 3;

	vector<GLfloat> flat_data;

	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(2);
	flat_data.push_back(0);
	flat_data.push_back(0);

	glUniform3f(uniforms.flat.flat_colour, 1.0, 0.0, 0.0);

	GLuint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(render.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(render.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_LINES, 0, num_vertices);

	flat_data.clear();

	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(2);
	flat_data.push_back(0);

	glUniform3f(uniforms.flat.flat_colour, 0.0, 1.0, 0.0);

	//glDeleteBuffers(1, &axis_buffer);
	//glGenBuffers(1, &axis_buffer);

	num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(render.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(render.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_LINES, 0, num_vertices);

	flat_data.clear();

	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(0);
	flat_data.push_back(2);

	glUniform3f(uniforms.flat.flat_colour, 0.0, 0.0, 1.0);

	//glDeleteBuffers(1, &axis_buffer);
	//glGenBuffers(1, &axis_buffer);

	num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(render.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(render.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_LINES, 0, num_vertices);

	glDeleteBuffers(1, &axis_buffer);
}

vec3 screen_coords_to_world_coords(const int x, const int y, const int screen_width, const int screen_height)
{
	const float half_screen_width = screen_width / 2.0f;
	const float half_screen_height = screen_height / 2.0f;

	mat4 inv_mat = inverse(main_camera.projection_mat * main_camera.view_mat);

	vec4 n((x - half_screen_width) / half_screen_width, -1 * (y - half_screen_height) / half_screen_height, -1, 1.0);
	vec4 f((x - half_screen_width) / half_screen_width, -1 * (y - half_screen_height) / half_screen_height, 1, 1.0);

	vec4 near_result = inv_mat * n;
	vec4 far_result = inv_mat * f;

	near_result /= near_result.w;
	far_result /= far_result.w;

	vec3 dir = vec3(far_result - near_result);

	vec3 ret_dir(dir.x, dir.y, dir.z);
	ret_dir = normalize(ret_dir);

	return ret_dir;
}


void take_screenshot2(size_t num_cams_wide, const char* filename)
{
	size_t ss_width = win_x * num_cams_wide;
	size_t ss_height = win_y * num_cams_wide;

	glViewport(0, 0, ss_width, ss_height);

	GLuint      fbo = 0;
	GLuint      fbo_tex[3] = { 0, 0, 0 };

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(3, fbo_tex);

	glBindTexture(GL_TEXTURE_2D, fbo_tex[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, ss_width, ss_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, fbo_tex[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, ss_width, ss_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, fbo_tex[2]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, ss_width, ss_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo_tex[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fbo_tex[1], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo_tex[2], 0);

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, draw_buffers);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(render.get_program());

	const GLfloat background_colour[] = { 1.0f, 0.5f, 0.0f, 0.0f };
	static const GLfloat one = 1.0f;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);

	glClearBufferfv(GL_COLOR, 0, background_colour);
	glClearBufferfv(GL_COLOR, 1, background_colour);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);

	draw_axis();
	draw_mesh();

	glUseProgram(ssao.get_program());

	glUniform1f(uniforms.ssao.ssao_radius, ssao_radius * float(ss_width) / 1000.0f);
	glUniform1f(uniforms.ssao.ssao_level, show_ao ? (show_shading ? 0.3f : 1.0f) : 0.0f);
	glUniform1i(uniforms.ssao.weight_by_angle, weight_by_angle ? 1 : 0);
	glUniform1i(uniforms.ssao.randomize_points, randomize_points ? 1 : 0);
	glUniform1ui(uniforms.ssao.point_count, point_count);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_tex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_tex[1]);

	glGenVertexArrays(1, &quad_vao);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteVertexArrays(1, &quad_vao);



	vector<unsigned char> output_pixels(ss_width * ss_height * 3);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, ss_width, ss_height, GL_RGB, GL_UNSIGNED_BYTE, &output_pixels[0]);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Set up Targa TGA image data.
	unsigned char  idlength = 0;
	unsigned char  colourmaptype = 0;
	unsigned char  datatypecode = 2;
	unsigned short int colourmaporigin = 0;
	unsigned short int colourmaplength = 0;
	unsigned char  colourmapdepth = 0;
	unsigned short int x_origin = 0;
	unsigned short int y_origin = 0;

	unsigned short int px = ss_width;
	unsigned short int py = ss_height;
	unsigned char  bitsperpixel = 24;
	unsigned char  imagedescriptor = 0;
	vector<char> idstring;

		for (size_t i = 0; i < ss_width; i++)
		{
			for (size_t j = 0; j < ss_height; j++)
			{
				size_t index = 3 * (j * ss_width + i);

				unsigned char temp_char;
				temp_char = output_pixels[index + 0];
				output_pixels[index + 0] = output_pixels[index + 2];
				output_pixels[index + 2] = temp_char;
			}
		}

	// Write Targa TGA file to disk.
	ofstream out(filename, ios::binary);

	if (!out.is_open())
	{
		cout << "Failed to open TGA file for writing: " << filename << endl;
		return;
	}

	out.write(reinterpret_cast<char*>(&idlength), 1);
	out.write(reinterpret_cast<char*>(&colourmaptype), 1);
	out.write(reinterpret_cast<char*>(&datatypecode), 1);
	out.write(reinterpret_cast<char*>(&colourmaporigin), 2);
	out.write(reinterpret_cast<char*>(&colourmaplength), 2);
	out.write(reinterpret_cast<char*>(&colourmapdepth), 1);
	out.write(reinterpret_cast<char*>(&x_origin), 2);
	out.write(reinterpret_cast<char*>(&y_origin), 2);
	out.write(reinterpret_cast<char*>(&px), 2);
	out.write(reinterpret_cast<char*>(&py), 2);
	out.write(reinterpret_cast<char*>(&bitsperpixel), 1);
	out.write(reinterpret_cast<char*>(&imagedescriptor), 1);

	out.write(reinterpret_cast<char*>(&output_pixels[0]), ss_width * ss_height * 3 * sizeof(unsigned char));

	out.close();


	glViewport(0, 0, win_x, win_y);


	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(3, fbo_tex);
}



#endif