#include "MeshHB.h"


MeshHB::MeshHB(vector<Vertex> vertices, vector<GLuint> indicies, vector<Texture> texture)
{
	this->vertices = vertices;
	this->indices = indicies;
	this->textures = texture;

	this->setupMesh();
}

void MeshHB::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);		// vertex data structure, needs to be configured
	glGenBuffers(1, &this->VBO);			// vertex data source
	glGenBuffers(1, &this->EBO);			// element data structure

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),	// Number of vertices * Vertex struct will be allocated allocates memory and stores data
		&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), 
		&this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)0);
						
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex,Normal));
						
	// Vertex Texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex,TexCoords));

	glBindVertexArray(0);		// unbind when done configuring
}

void MeshHB::Draw(Shader shader) 
{
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);	// activate before binidng
		stringstream ss;
		string number;
		string name = this->textures[i].type;
		if (name == "texture_diffuse")
			ss << diffuseNr++;
		else if (name == "texture_specular")
			ss << specularNr++;
		number = ss.str();
														// if type is texture_diffuse and first then
														// material.texture_diffuse1
														// cannot explain why i is there though
		glUniform1f(glGetUniformLocation(shader.program, ("material." + name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(this -> VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
MeshHB::~MeshHB()
{
}
