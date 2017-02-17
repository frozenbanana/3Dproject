#ifndef SHADERJM_H
#define SHADERJM_H

#include <Windows.h>

#include <stdio.h>
#include <string>
#include <fstream>
#include <streambuf>

#include <GL\glew.h>

class JMshader {
private:

	void createShader(const GLuint* shaderID, const GLchar* path);

public:
	//Variables
	GLuint program;		//ID for shader program

	//Functions
	JMshader(const GLchar* verPath, const GLchar* geoPath, const GLchar* fraPath);		//Constructor that takes file-paths from which the shaders should be read
	JMshader(const GLchar* verPath, const GLchar* fraPath);		//For shader programs with no geometry shader

	void useProgram();		//Calls for the programs use
};


#endif