#pragma once
#include <vector>
#include <iostream>
#include <HB\meshHB.h>

// asimp
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

// SOIL
#include <SOIL.h>

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

