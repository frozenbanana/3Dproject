#ifndef MODELJM_H
#define MODELJM_H

#include <vector>
#include <string>

#include <GL\glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SOIL.h>

#include "meshJM.h"
#include "shaderJM.h"

class JMmodel {
private:
	//Data storage
	std::vector<JMmesh> meshes;
	std::string dir;
	std::vector<JMtexture> loadedTextures;

	//Functions
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	JMmesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<JMtexture> loadMatTex(aiMaterial* mat, aiTextureType type, std::string typeName);
	GLuint loadTexFile(const char* path, std::string dir);

public:
	JMmodel(GLchar* path);
	void drawModel(JMshader* shaderPtr);
};

#endif