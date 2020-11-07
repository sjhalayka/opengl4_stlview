#include "vertex_fragment_shader.h"


bool vertex_fragment_shader::init(const char *vertex_shader_filename, const char *fragment_shader_filename)
{
	program = glCreateProgram();

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* source = read_text_file(vertex_shader_filename);

	if(source == NULL) 
	{
		glDeleteProgram(program);
		glDeleteShader(vertex_shader);
		return false;
	}

	glShaderSource(vertex_shader, 1, &source, NULL);
	delete [] source;

	glCompileShader(vertex_shader);
	GLint compiled;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);

	if(!compiled)
	{
		GLsizei len;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len+1];
		glGetShaderInfoLog(vertex_shader, len, &len, log);
		cerr << "Shader compilation failed: " << log << endl;
		delete [] log;

		glDeleteProgram(program);
		glDeleteShader(vertex_shader);
		return false;
	}

	glAttachShader(program, vertex_shader);



	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	source = read_text_file(fragment_shader_filename);

	if(source == NULL) 
	{
		glDeleteProgram(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return false;
	}

	glShaderSource(fragment_shader, 1, &source, NULL);
	delete [] source;

	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

	if(!compiled)
	{
		GLsizei len;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = new GLchar[len+1];
		glGetShaderInfoLog(fragment_shader, len, &len, log);
		cerr << "Shader compilation failed: " << log << endl;
		delete [] log;

		glDeleteProgram(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return false;
	}

	glAttachShader(program, fragment_shader);
	


    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if(!linked)
	{
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog(program, len, &len, log);
        cerr << "Shader linking failed: " << log << endl;
        delete [] log;

		glDeleteProgram(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
        
        return false;
    }

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

    return true;	
}



#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

const GLchar* vertex_fragment_shader::read_text_file(const char* filename)
{
	FILE* infile = fopen(filename, "rb");

	if(!infile)
	{
		cerr << "Unable to open file '" << filename << "'" << endl;
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = new GLchar[len+1];

	fread(source, sizeof(char), len, infile);
	fclose(infile);

	source[len] = 0;

	return const_cast<const GLchar*>(source);
}

void vertex_fragment_shader::use_program(void)
{
	glUseProgram(program);
}