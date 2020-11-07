#include "main.h"
#include "mesh.h"


int main(int argc, char **argv)
{
	if(argc != 2)
	{
		cout << "Example usage: stlview filename.stl" << endl;
		return 1;
	}
	
	cout << "Reading file: " << argv[1] << endl;
	
	if(false == read_triangles_from_binary_stereo_lithography_file(triangles, argv[1]))
	{
		cout << "Error: Could not properly read file " << argv[1] << endl;
		return 2;
	}
	
	scale_mesh(triangles, 2.0f);
	
	get_vertices_and_normals_from_triangles(triangles, face_normals, vertices, vertex_normals);	
	
	glutInit(&argc, argv);
	
	if (false == init_opengl(win_x, win_y))
		return 1;

	glutReshapeFunc(reshape_func);
	glutIdleFunc(idle_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutPassiveMotionFunc(passive_motion_func);
	//glutIgnoreKeyRepeat(1);
	glutMainLoop();
	glutDestroyWindow(win_id);

	return 0;
}



void idle_func(void)
{
	glutPostRedisplay();
}

bool init_opengl(const int &width, const int &height)
{
	win_x = width;
	win_y = height;

	if(win_x < 1)
		win_x = 1;

	if(win_y < 1)
		win_y = 1;

	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("Binary Stereo Lithography file viewer");

	if (GLEW_OK != glewInit())
	{
		cout << "GLEW initialization error" << endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (false == render.init("render.vs.glsl", "render.gs.glsl", "render.fs.glsl"))
	{
		cout << "Could not load render shader" << endl;
		return false;
	}

	uniforms.render.mv_matrix = glGetUniformLocation(render.get_program(), "mv_matrix");
	uniforms.render.proj_matrix = glGetUniformLocation(render.get_program(), "proj_matrix");
	uniforms.render.shading_level = glGetUniformLocation(render.get_program(), "shading_level");

	if (false == flat.init("flat.vs.glsl", "flat.fs.glsl"))
	{
		cout << "Could not load flat shader" << endl;
		return false;
	}

	uniforms.flat.mv_matrix = glGetUniformLocation(flat.get_program(), "mv_matrix");
	uniforms.flat.proj_matrix = glGetUniformLocation(flat.get_program(), "proj_matrix");
	uniforms.flat.flat_colour = glGetUniformLocation(flat.get_program(), "flat_colour");


	if (false == ssao.init("ssao.vs.glsl", "ssao.fs.glsl"))
	{
		cout << "Could not load SSAO shader" << endl;
		return false;
	}

	uniforms.ssao.ssao_radius = glGetUniformLocation(ssao.get_program(), "ssao_radius");
	uniforms.ssao.ssao_level = glGetUniformLocation(ssao.get_program(), "ssao_level");
	uniforms.ssao.object_level = glGetUniformLocation(ssao.get_program(), "object_level");
	uniforms.ssao.weight_by_angle = glGetUniformLocation(ssao.get_program(), "weight_by_angle");
	uniforms.ssao.randomize_points = glGetUniformLocation(ssao.get_program(), "randomize_points");
	uniforms.ssao.point_count = glGetUniformLocation(ssao.get_program(), "point_count");

	

	ssao_level = 1.0f;
	ssao_radius = 0.05f;
	show_shading = true;
	show_ao = true;
	weight_by_angle = true;
	randomize_points = true;
	point_count = 10;

	glGenFramebuffers(1, &render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glGenTextures(3, fbo_textures);

	glBindTexture(GL_TEXTURE_2D, fbo_textures[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, fbo_textures[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, fbo_textures[2]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo_textures[0], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fbo_textures[1], 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo_textures[2], 0);

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, draw_buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	SAMPLE_POINTS point_data;

	for (size_t i = 0; i < 256; i++)
	{
		do
		{
			point_data.point[i].x = random_float() * 2.0f - 1.0f;
			point_data.point[i].y = random_float() * 2.0f - 1.0f;
			point_data.point[i].z = random_float(); //  * 2.0f - 1.0f;
			point_data.point[i].w = 0.0f;
		}
		while (length(point_data.point[i]) > 1.0f);

		point_data.point[i] = normalize(point_data.point[i]);
	}

	for (size_t i = 0; i < 256; i++)
	{
		point_data.random_vectors[i].x = random_float();
		point_data.random_vectors[i].y = random_float();
		point_data.random_vectors[i].z = random_float();
		point_data.random_vectors[i].w = random_float();
	}

	glGenBuffers(1, &points_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, points_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);


	return true;
}

void reshape_func(int width, int height)
{
	win_x = width;
	win_y = height;

	if(win_x < 1)
		win_x = 1;

	if(win_y < 1)
		win_y = 1;

	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);
}

void draw_dot(void)
{
	glUseProgram(flat.get_program());

	glUniformMatrix4fv(uniforms.flat.proj_matrix, 1, GL_FALSE, &main_camera.projection_mat[0][0]);
	glUniformMatrix4fv(uniforms.flat.mv_matrix, 1, GL_FALSE, &main_camera.view_mat[0][0]);
	glUniform3f(uniforms.flat.flat_colour, 1.0, 0.0, 0.0);

	GLint components_per_vertex = 3;
	GLint components_per_position = 3;

	glPointSize(4.0f);

	vector<float> flat_data;

	vec3 eye(main_camera.eye.x, main_camera.eye.y, main_camera.eye.z);

	vec3 pos = eye + ray;

	flat_data.push_back(pos.x);
	flat_data.push_back(pos.y);
	flat_data.push_back(pos.z);

	GLuint point_buffer;

	glGenBuffers(1, &point_buffer);

	GLint num_vertices = static_cast<GLuint>(flat_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, point_buffer);
	glBufferData(GL_ARRAY_BUFFER, flat_data.size() * sizeof(GLfloat), &flat_data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(flat.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(flat.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glDrawArrays(GL_POINTS, 0, num_vertices);

	glDeleteBuffers(1, &point_buffer);
}

void draw_mesh(void)
{
	glUseProgram(render.get_program());

	main_camera.calculate_camera_matrices(win_x, win_y);
	glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, &main_camera.projection_mat[0][0]);
	glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, &main_camera.view_mat[0][0]);
	glUniform1f(uniforms.render.shading_level, 1.0f);

	vector<float> vertex_data;

	for (size_t i = 0; i < triangles.size(); i++)
	{
		vec3 colour(0.0f, 0.8f, 1.0f);

		size_t v0_index = triangles[i].vertex[0].index;
		size_t v1_index = triangles[i].vertex[1].index;
		size_t v2_index = triangles[i].vertex[2].index;

		vec3 v0_fn(vertex_normals[v0_index].x, vertex_normals[v0_index].y, vertex_normals[v0_index].z);
		vec3 v1_fn(vertex_normals[v1_index].x, vertex_normals[v1_index].y, vertex_normals[v1_index].z);
		vec3 v2_fn(vertex_normals[v2_index].x, vertex_normals[v2_index].y, vertex_normals[v2_index].z);

		vec3 v0(triangles[i].vertex[0].x, triangles[i].vertex[0].y, triangles[i].vertex[0].z);
		vec3 v1(triangles[i].vertex[1].x, triangles[i].vertex[1].y, triangles[i].vertex[1].z);
		vec3 v2(triangles[i].vertex[2].x, triangles[i].vertex[2].y, triangles[i].vertex[2].z);

		vertex_data.push_back(v0.x);
		vertex_data.push_back(v0.y);
		vertex_data.push_back(v0.z);
		vertex_data.push_back(v0_fn.x);
		vertex_data.push_back(v0_fn.y);
		vertex_data.push_back(v0_fn.z);
		vertex_data.push_back(colour.x);
		vertex_data.push_back(colour.y);
		vertex_data.push_back(colour.z);

		vertex_data.push_back(v1.x);
		vertex_data.push_back(v1.y);
		vertex_data.push_back(v1.z);
		vertex_data.push_back(v1_fn.x);
		vertex_data.push_back(v1_fn.y);
		vertex_data.push_back(v1_fn.z);
		vertex_data.push_back(colour.x);
		vertex_data.push_back(colour.y);
		vertex_data.push_back(colour.z);

		vertex_data.push_back(v2.x);
		vertex_data.push_back(v2.y);
		vertex_data.push_back(v2.z);
		vertex_data.push_back(v2_fn.x);
		vertex_data.push_back(v2_fn.y);
		vertex_data.push_back(v2_fn.z);
		vertex_data.push_back(colour.x);
		vertex_data.push_back(colour.y);
		vertex_data.push_back(colour.z);
	}


	GLuint components_per_vertex = 9;
	const GLuint components_per_normal = 3;
	GLuint components_per_position = 3;
	const GLuint components_per_colour = 3;

	GLuint triangle_buffer;

	glGenBuffers(1, &triangle_buffer);

	GLuint num_vertices = static_cast<GLuint>(vertex_data.size()) / components_per_vertex;

	glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(GLfloat), &vertex_data[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(render.get_program(), "position"));
	glVertexAttribPointer(glGetAttribLocation(render.get_program(), "position"),
		components_per_position,
		GL_FLOAT,
		GL_FALSE,
		components_per_vertex * sizeof(GLfloat),
		NULL);

	glEnableVertexAttribArray(glGetAttribLocation(render.get_program(), "normal"));
	glVertexAttribPointer(glGetAttribLocation(render.get_program(), "normal"),
		components_per_normal,
		GL_FLOAT,
		GL_TRUE,
		components_per_vertex * sizeof(GLfloat),
		(const GLvoid*)(components_per_position * sizeof(GLfloat)));

	glEnableVertexAttribArray(glGetAttribLocation(render.get_program(), "colour"));
	glVertexAttribPointer(glGetAttribLocation(render.get_program(), "colour"),
		components_per_colour,
		GL_FLOAT,
		GL_TRUE,
		components_per_vertex * sizeof(GLfloat),
		(const GLvoid*)(components_per_normal * sizeof(GLfloat) + components_per_position * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	glDeleteBuffers(1, &triangle_buffer);









}


void draw_axis(void)
{
	unsigned int axis_buffer = 0;
	glGenBuffers(1, &axis_buffer);

	glLineWidth(2.0);

	glUseProgram(flat.get_program());

	main_camera.calculate_camera_matrices(win_x, win_y);
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


void display_func(void)
{
	glEnable(GL_DEPTH_TEST);

	glUseProgram(render.get_program());

	const GLfloat background_colour[] = { 1.0f, 0.5f, 0.0f, 0.0f };
	static const GLfloat one = 1.0f;
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glEnable(GL_DEPTH_TEST);

	glClearBufferfv(GL_COLOR, 0, background_colour);
	glClearBufferfv(GL_COLOR, 1, background_colour);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);

	draw_axis();

	draw_mesh();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(ssao.get_program());

	glUniform1f(uniforms.ssao.ssao_radius, ssao_radius * float(win_x) / 1000.0f);
	glUniform1f(uniforms.ssao.ssao_level, show_ao ? (show_shading ? 0.3f : 1.0f) : 0.0f);
	glUniform1i(uniforms.ssao.weight_by_angle, weight_by_angle ? 1 : 0);
	glUniform1i(uniforms.ssao.randomize_points, randomize_points ? 1 : 0);
	glUniform1ui(uniforms.ssao.point_count, point_count);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_textures[1]);

	glGenVertexArrays(1, &quad_vao);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteVertexArrays(1, &quad_vao);


	draw_dot();




	glFlush();
	glutSwapBuffers();
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch(tolower(key))
	{
	case 'a':
		break;

	default:
		break;
	}
}

void mouse_func(int button, int state, int x, int y)
{
	if(GLUT_LEFT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
		{
			ray = screen_coords_to_world_coords(x, y, win_x, win_y);

			lmb_down = true;
		}
		else
			lmb_down = false;
	}
	else if(GLUT_MIDDLE_BUTTON == button)
	{
		if(GLUT_DOWN == state)
			mmb_down = true;
		else
			mmb_down = false;
	}
	else if(GLUT_RIGHT_BUTTON == button)
	{
		if(GLUT_DOWN == state)
			rmb_down = true;
		else
			rmb_down = false;
	}
}

void motion_func(int x, int y)
{
	int prev_mouse_x = mouse_x;
	int prev_mouse_y = mouse_y;

	mouse_x = x;
	mouse_y = y;

	int mouse_delta_x = mouse_x - prev_mouse_x;
	int mouse_delta_y = prev_mouse_y - mouse_y;

	if(true == lmb_down && (0 != mouse_delta_x || 0 != mouse_delta_y))
	{
		//cout << main_camera.eye.x << ' ' << main_camera.eye.y << ' ' << main_camera.eye.z << endl;
		//cout << main_camera.look_at.x << ' ' << main_camera.look_at.y << ' ' << main_camera.look_at.z << endl;
		//cout << ray.x << ' ' << ray.y << ' ' << ray.z << endl;
		//cout << endl;

		main_camera.u -= static_cast<float>(mouse_delta_y)*u_spacer;
		main_camera.v += static_cast<float>(mouse_delta_x)*v_spacer;

		main_camera.calculate_camera_matrices(win_x, win_y);
	}
	else if(true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y)*w_spacer;

		if(main_camera.w < 1.1f)
			main_camera.w = 1.1f;
		else if(main_camera.w > 20.0f)
			main_camera.w = 20.0f;

		main_camera.calculate_camera_matrices(win_x, win_y);
	}
}

void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}




