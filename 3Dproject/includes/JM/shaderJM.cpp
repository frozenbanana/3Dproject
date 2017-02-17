#include "shaderJM.h"

//Private
void JMshader::createShader(const GLuint* shaderID, const GLchar* path) {

	std::ifstream shaderFile(path);
	std::string glslText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	const char* glslPtr = glslText.c_str();
	glShaderSource(*shaderID, 1, &glslPtr, NULL);
	glCompileShader(*shaderID);

	GLint shaderSuccess = 0;
	glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &shaderSuccess);
	if (shaderSuccess == GL_FALSE) {
		GLchar shader_log_string[1024] = {};
		glGetShaderInfoLog(*shaderID, 1024, NULL, shader_log_string);
		//OutputDebugStringA(verPath);
		OutputDebugStringA((char*)shader_log_string);
	}

}

//Public
JMshader::JMshader(const GLchar* verPath, const GLchar* geoPath, const GLchar* fraPath) {

	//create vertex shader
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	this->createShader(&vShader, verPath);

	//create geometry shader
	GLuint gShader = glCreateShader(GL_GEOMETRY_SHADER);
	this->createShader(&gShader, geoPath);

	//create fragment shader
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	this->createShader(&fShader, fraPath);

	//attach the shaders to the program
	this->program = glCreateProgram();
	glAttachShader(this->program, vShader);
	glAttachShader(this->program, gShader);
	glAttachShader(this->program, fShader);
	glLinkProgram(this->program);

	//Delete not needed?
}

JMshader::JMshader(const GLchar* verPath, const GLchar* fraPath) {

	//create vertex shader
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	this->createShader(&vShader, verPath);

	//create fragment shader
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	this->createShader(&fShader, fraPath);

	//attach the shaders to the program
	this->program = glCreateProgram();
	glAttachShader(this->program, vShader);
	glAttachShader(this->program, fShader);
	glLinkProgram(this->program);

	//Delete not needed?
}

void JMshader::useProgram() {
	glUseProgram(this->program);
}