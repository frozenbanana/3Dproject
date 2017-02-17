#include "meshJM.h"

//Private
void JMmesh::setupMesh() {
	//Enable VAO
	glGenVertexArrays(1, &(this->vao));
	glBindVertexArray(this->vao);

	//Create VBO id
	glGenBuffers(1, &(this->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(JMvertex), &(this->vertices)[0], GL_STATIC_DRAW);

	//EBO? //NTS: Check this out //NTS: Keeps track of indices and which vertices that belong to were
	glGenBuffers(1, &(this->ebo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &(this->indices)[0], GL_STATIC_DRAW);

	//Tell the program where to find the vertex postition
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(JMvertex), (GLvoid*)0);
																		//The last argument is something akin to BUFFER_OFFSET. Check it out closer
	//Tell the program where to find the vertex normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(JMvertex), (GLvoid*)offsetof(JMvertex, norm));
																		//Keep in mind that a stucts data lies sequentially in the memory (like an array)
																		//The offsetof(s,a) takes an struct and a variable name from within the struct
																		//It then returns the byte offset to that variables position.
	//Tell the program where to find the vertex texture uv:s
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(JMvertex), (GLvoid*)offsetof(JMvertex, uvs));

	//After this the program expects to find 3 floats at position 0, 3 floats at position 1 and 2 floats at position 2

	//TEST: Tell the program where to find colors ((Find X):(on Y))
	//Diffuse:3
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(JMvertex), (GLvoid*)offsetof(JMvertex, difCol));

	//Specular:4
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(JMvertex), (GLvoid*)offsetof(JMvertex, speCol));

	//Ambient:5
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(JMvertex), (GLvoid*)offsetof(JMvertex, ambCol));

	glBindVertexArray(0);

}

//Public
JMmesh::JMmesh(std::vector<JMvertex> inVertices, std::vector<GLuint> inIndices, std::vector<JMtexture> inTextures) {
	//Data for the mesh
	vertices = inVertices;
	indices = inIndices;
	textures = inTextures;

	this->setupMesh();
}

void JMmesh::drawMesh(JMshader* shaderPtr) {
	
	GLuint diffNum = 1;
	GLuint specNum = 1;
	
	for (size_t i = 0; i < this->textures.size(); i++) {	//NTS: calling size() on a vector returns number of elements in it, not maximum capacity
		
		glActiveTexture(GL_TEXTURE0 + i);				//Activate the correct texture. Assumption: The GL_TEXTURE0 is equal to the identfier of
														//'0' and when adding the GLuint 'i' to it converts 'i' into the same type

		//Below here, the code is following the naming convention that each uniform is
		//named "texture_diffuseN" or "texture_specularN" where N is the number of the
		//uniform. This is what was suggested in learnopengl's tutorial on meshes.
		//Consider if this should be changed to something else

		std::string name = (this->textures[i]).texType;
		std::string num = "";

		if (name == "texture_diffuse") {
			num = std::to_string(diffNum);
			diffNum++;
		}
		else if (name == "texture_specular") {
			num = std::to_string(specNum);
			specNum++;
		}
		else {
			//NTS: What to do if the name is wrong? Stop the program? Handle exceptions?
			//Do not solve this until it has been made sure that the naming convention above is what shall be followed
		}

		//Get the uniform location of the texture for the shader program under the name "[type of texture][number]"
		//Upload that uniform and set the uniform variable to the number of the texture
		GLuint temp = glGetUniformLocation((*shaderPtr).program, (name + num).c_str());
		if (temp == (GLuint)-1)
		{
			//ERROR
			OutputDebugString("ERROR: glGetUniformLocation returned a faulty value!");
		}

		glUniform1i(temp, (GLint)i);
		glBindTexture(GL_TEXTURE_2D, (this->textures[i]).texId);

	}

	glActiveTexture(GL_TEXTURE0);


	//Draw the mesh
	glBindVertexArray(this->vao);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//Reset to default in order to avoid future complications
	for (size_t i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}