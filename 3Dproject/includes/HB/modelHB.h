#pragma once
#include <vector>
#include <iostream>
#include "MeshHB.h"

// asimp
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

// SOIL
#include "SOIL2\SOIL2.h"

class ModelHB
{
private:
	void loadModelHB(string path);
	void processNode(aiNode* node, const aiScene* scene);
	MeshHB processMeshHB(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	GLint TextureFromFile(const char* path, string directory, bool gamma = false);
public:
	vector<Texture> textures_loaded;
	vector<MeshHB> meshes;
	string directory;
	ModelHB(GLchar* path);
	void Draw(Shader shader);
	~ModelHB();
};

