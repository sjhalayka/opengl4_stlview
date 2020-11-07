#ifndef VERTEX_FRAGMENT_SHADER
#define VERTEX_FRAGMENT_SHADER

#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <vector>
using namespace std;


class vertex_fragment_shader
{
public:

	vertex_fragment_shader(void) { program = 0; }
	~vertex_fragment_shader(void) { if(program !=0) {glDeleteProgram(program);} }

	bool init(const char *vertex_shader_filename, const char *fragment_shader_filename);
	void use_program(void);
	GLuint get_program(void) { return program; };

private: 
	const GLchar* read_text_file(const char* filename);
	GLuint program;

};



#endif