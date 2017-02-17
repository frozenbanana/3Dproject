#ifndef MESHJM_H
#define MESHJM_H


#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include <assimp\types.h>		//Required for aiString
#include <string>
#include <vector>

#include "shaderJM.h"


//Structs for handling vertices
struct JMvertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uvs;

	glm::vec4 difCol;		//Diffuse Color
	glm::vec4 speCol;		//Specular Color
	glm::vec4 ambCol;		//Ambient Color
};

struct JMtexture {
	GLuint texId;
	std::string texType;
	aiString texPath;	// needs assimp
};

//The mesh class
class JMmesh {
private:
	//Variables for rendering
	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	//Data for the mesh
	std::vector<JMvertex> vertices;
	std::vector<GLuint> indices;
	std::vector<JMtexture> textures;

	//Functions
	void setupMesh();
public:
	JMmesh(std::vector<JMvertex> inVertices, std::vector<GLuint> inIndices, std::vector<JMtexture> inTextures);
	
	void drawMesh(JMshader* shaderPtr);

	//Functions for getting the vertices of the mesh
	//std::vector<JMvertex> getVertices();
	//std::vector<GLuint> getIndices();
	//std::vector<JMtexture> getTextures();

};




#endif