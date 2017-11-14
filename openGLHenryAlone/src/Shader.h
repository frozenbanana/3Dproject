#pragma once
#include <string>
#include <GL\glew.h>
#include "Transform.h"
#include "Camera.h"

class Shader
{
public:
	Shader(const std::string& fileName);

	void Bind();
	void Update(const Transform& transform, const Camera& camera);
	
	~Shader();
private:
	static const unsigned int NUM_SHADER = 2;	// this needs to be changed to manage compute shader and geometry shader
	//static const unsigned int NUM_UNIFORMS = 3;

	std::string LoadShader(const std::string& fileName);
	void CheckShaderError(GLuint shader, GLint flag, bool isProgram, const std::string& errorMessage);
	GLuint CreateShader(const std::string& text, unsigned int type);

	enum {
		TRANSFORM_U,
		
		NUM_UNIFORMS
	};

	GLuint m_program;
	GLuint m_shaders[NUM_SHADER];
	GLuint m_uniforms[NUM_UNIFORMS];
};

