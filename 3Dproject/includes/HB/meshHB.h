#pragma once

#include "Shader.h"

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// asimp
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	string type;
	aiString path;	// Storing the patch to compare with other textures
};

class MeshHB
{
private:
	GLuint VAO, VBO, EBO;

	void setupMesh();

public:
	// Mesh data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	
	MeshHB(vector<Vertex> vertices, vector<GLuint> indicies, vector<Texture> texture);
	void Draw(Shader shader);
	~MeshHB();
};

