#include "ModelHB.h"

ModelHB::ModelHB(GLchar* path)
{
	this->loadModelHB(path);
}

void ModelHB::Draw(Shader shader)
{
	for (GLuint i = 0; i < meshes.size(); i++)
	{
		this->meshes[i].Draw(shader);
	}
}
// recursive function
void ModelHB::processNode(aiNode* node, const aiScene* scene)
{


	if (scene->mNumMeshes == 0)
	{
		cout << "Input file contains no meshes." << endl;
	}
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMeshHB(mesh, scene));
	}

	// then same for children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}
MeshHB ModelHB::processMeshHB(aiMesh* mesh, const aiScene* scene)
{
	// mesh data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	// Transfering vertex data from assimp
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 tempVector;

		// Position
		tempVector.x = mesh->mVertices[i].x;
		tempVector.y = mesh->mVertices[i].y;
		tempVector.z = mesh->mVertices[i].z;
		vertex.Position = tempVector;		// transfer texture data
		// Normal
		tempVector.x = mesh->mNormals[i].x;
		tempVector.y = mesh->mNormals[i].y;
		tempVector.z = mesh->mNormals[i].z;
		vertex.Normal = tempVector;			// transfer texture data
		// Texture
		if (mesh->mTextureCoords[0]) // check if the mesh has texture coords
		{
			glm::vec2 tempVectorTex;
			tempVectorTex.x = mesh->mTextureCoords[0][i].x; // assimp allows 8 different textures per mesh, we online need the first - [0]
			tempVectorTex.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = tempVectorTex; // transfer texture data 
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

		// indices
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// material
		if (mesh->mMaterialIndex >= 0)
		{		// why >0 and not >
			
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
	

	// Return a mesh object created from the extracted mesh data
	return MeshHB(vertices, indices, textures);
}
vector<Texture> ModelHB::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{// If texture hasn't been loaded already, load it
			aiString str;
			mat->GetTexture(type, i, &str);
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			textures.push_back(texture);
		}
	}
	return textures;
}

void ModelHB::loadModelHB(string path)
{
	// READ FILE VIA ASSIMP
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)				// check if scene and root node is not NULL
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	this->directory = path.substr(0, path.find_last_of('/'));
	cout << this->directory << endl;
	this->processNode(scene->mRootNode, scene);													// process every node recursively
}

ModelHB::~ModelHB()
{

}

GLint ModelHB::TextureFromFile(const char* path, string directory, bool gamma)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, gamma ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}