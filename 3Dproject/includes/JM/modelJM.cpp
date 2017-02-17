#include "modelJM.h"

//Private
void JMmodel::loadModel(std::string path) {
	//Set up the assimp importer object
	Assimp::Importer importerObject;

	//Read from the file (assimp handles the file-type) at the destination of 'path'
	//The aiProcess_Triangulate ensures that what is read from the file can be seen as triangles
	//The aiProcess_FlipUVs fixes the change in coordinates that comes with OpenGL
	//NTS: There is a command that creates normals if there aren't any (aiProcess_GenNormal)
	const aiScene* scene = importerObject.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	//The following if statement checks so that
	//The scene is not NULL
	//OR
	//That the scene has a flag and that the setup was finished (by checking flag)
	//OR
	//The root node of the scene is not NULL
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		//ERROR
		OutputDebugString("ERROR: The scene was not created as intended!");
		return;
	}

	//Read the filepath to find the model
	this->dir = path.substr(0, path.find_last_of('/'));

	//This is an recursive function that goes through the nodes and their children. It starts in the root node
	this->processNode(scene->mRootNode, scene);
}

void JMmodel::processNode(aiNode* node, const aiScene* scene) {
	//If the node hold any meshes they are to be processed first
	for (int i = 0; i < node->mNumMeshes; i++) {
		//Set a pointer to point at the scene's meshes. The index is gotten from the current node
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		//In the vector of meshes, push back (that is, add another element to the end) the mesh that
		//is returned from the processMesh function
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	//Now go though the children of the node, if there are any
	for (int i = 0; i < node->mNumChildren; i++) {
		//Here we simply call the recursive function again but with a child node as indata
		this->processNode(node->mChildren[i], scene);
	}
}

JMmesh JMmodel::processMesh(aiMesh* mesh, const aiScene* scene) {
	//Variables
	std::vector<JMvertex> verVec;
	std::vector<GLuint> indVec;
	std::vector<JMtexture> texVec;

	//-------------------------------------------------------------------------------------------------------

	//Vertices
	for (int i = 0; i < mesh->mNumVertices; i++) {
		JMvertex verTemp;						//Create a temporary vertex
		
		//Retrieve position
		glm::vec3 vecTemp;						//Create an temporarty vector
		vecTemp.x = mesh->mVertices[i].x;		//Get the x, y and z coordinates from the current vertex in the mesh
		vecTemp.y = mesh->mVertices[i].y;
		vecTemp.z = mesh->mVertices[i].z;
		verTemp.pos = vecTemp;					//Save it down within the pos vector within the vertex

		//Retrieve normal
		vecTemp.x = mesh->mNormals[i].x;		//Note that vecTemp is used again anf the data within it overwritten
		vecTemp.y = mesh->mNormals[i].y;
		vecTemp.z = mesh->mNormals[i].z;
		verTemp.norm = vecTemp;					//Save down the normals

		//Retrieve the color of the material of the vertex
		aiColor4D colHolder;
		(scene->mMaterials[mesh->mMaterialIndex])->Get(AI_MATKEY_COLOR_DIFFUSE, colHolder);
		verTemp.difCol.x = colHolder.r;
		verTemp.difCol.y = colHolder.g;
		verTemp.difCol.z = colHolder.b;
		verTemp.difCol.a = colHolder.a;

		(scene->mMaterials[mesh->mMaterialIndex])->Get(AI_MATKEY_COLOR_SPECULAR, colHolder);
		verTemp.speCol.x = colHolder.r;
		verTemp.speCol.y = colHolder.g;
		verTemp.speCol.z = colHolder.b;
		verTemp.speCol.a = colHolder.a;

		(scene->mMaterials[mesh->mMaterialIndex])->Get(AI_MATKEY_COLOR_AMBIENT, colHolder);
		verTemp.ambCol.x = colHolder.r;
		verTemp.ambCol.y = colHolder.g;
		verTemp.ambCol.z = colHolder.b;
		verTemp.ambCol.a = colHolder.a;



		//Retrieve texture uvs
		//First check if there is any texture coordinates
		//mTextureCoords[X] is null if there are no texture coordinates
		if (mesh->mTextureCoords[0]) {
			//NTS: Assimp allows up to 8 texture coordinates for a model
			glm::vec2 vecTemp2;
			vecTemp2.x = mesh->mTextureCoords[0][i].x;	//NTS: Why [0][i]? //The 0 tells that it should be the first set of texture uvs
			vecTemp2.y = mesh->mTextureCoords[0][i].y;
			verTemp.uvs = vecTemp2;
		}
		else {
			//If there aren't any texture coordinates set them to be 0,0
			verTemp.uvs = glm::vec2(0.0f, 0.0f);
		}

		verVec.push_back(verTemp);				//Push the finished vertex onto the vector
	}

	//-------------------------------------------------------------------------------------------------------

	//Indices
	//Since this is run after the aiProcess_Triangulate flag has been set, each primitive is a triangle here
	for (int i = 0; i < mesh->mNumFaces; i++) {
		//Looping over all faces gives the indice data that tells in which order the vertices should be drawn
		aiFace face = mesh->mFaces[i];

		//After each face the indices must be read in the correct order, so they are looped though as well and put into the vector
		for (int j = 0; j < face.mNumIndices; j++) {
			indVec.push_back(face.mIndices[j]);
		}
	}

	//-------------------------------------------------------------------------------------------------------

	/*---------------------------------------------------------------------------------------*/
	/*            THIS PART IS CURRENTLY INCOMPLETE. DO LOOK AT IT AGAIN LATER               */
	/*---------------------------------------------------------------------------------------*/
	//NTS: Fix so that textures are only loaded if they exist.


	//Material
	//First check if there are any material data
	if (mesh->mMaterialIndex >= 0) {
		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		
			//Send in the pointer to the material
			//Notify which type we deal with (here: diffuse)
			//Send in a text sting describing the type (this follows the name pattern that's descrbed in the mesh class)
			//Save the vector that loadMatTex returns.
		std::vector<JMtexture> diffMaps = this->loadMatTex(material, aiTextureType_DIFFUSE, "texture_diffuse");
		//Insert the elements from the beginning of diffMaps to the end of diffMaps into the end of texVec
		texVec.insert(texVec.end(), diffMaps.begin(), diffMaps.end());

		//Do the same for specular maps
		std::vector<JMtexture> specMaps = this->loadMatTex(material, aiTextureType_SPECULAR, "texture_specular");
		texVec.insert(texVec.end(), specMaps.begin(), specMaps.end());

		//After this the texture vector (texVec) will contain all the diffuse textures in a row and then all the specular textures in a row
		
		//What if a material doesn't have a texture?

	}

	return JMmesh(verVec, indVec, texVec);
}

std::vector<JMtexture> JMmodel::loadMatTex(aiMaterial* mat, aiTextureType type, std::string typeName) {
	
	//Create a variable that shall be returned with the correct texture
	std::vector<JMtexture> returnTex;

	//Loop through all the textures of a certain type within the material
	for (int i = 0; i < mat->GetTextureCount(type); i++) {

		//Save down the type (aiTextureType_[TYPE]) of texture on index i
		//aiStr will contain the path to the texture
		//NTS: 'path' might be only the file name if the file lies within the same folder.
		//If it lies in a sub folder 'path' tell which one
		aiString aiStr;
		mat->GetTexture(type, i, &aiStr);
		
		//Set up a variable that will be used to check if the texture
		//at the end of the path has already been loaded
		bool textureFound = false;

		//Loop though the amount of loaded textures as long as
		//The full number of textures hasn't been looped though
		//AND
		//The texture has not been found among the already loaded textures
		for (int j = 0; (j < this->loadedTextures.size()) && (textureFound == false); j++) {

			//If the path of an already loaded texture coresponds to the texture to be loaded
			if (this->loadedTextures[j].texPath == aiStr) {

				//Set the texture that shall be returned to be the already loaded texture
				returnTex.push_back(this->loadedTextures[j]);
				
				//Set it so that the "new" texture isn't loaded as it already exists
				textureFound = true;
			}

		}
		if (textureFound == false) {
			//If the texture did not exist, load it in
			JMtexture tex;
			tex.texId = loadTexFile(aiStr.C_Str(), this->dir);		//Load in the new texture from a file and save its ID
			tex.texType = typeName;				//Save the texture's type
			tex.texPath = aiStr;				//Save down the path to the texture
			returnTex.push_back(tex);			//Push the texture onto the vector that shall be returned
			
			//Add the newly loaded textured to the list of loaded ones
			this->loadedTextures.push_back(tex);
		}
	}

	//Finally return the vector with the texture to be loaded
	return returnTex;
}

GLuint JMmodel::loadTexFile(const char* path, std::string dir) {

	//NTS: Path is described as seen from the local folder. By adding the directory-path
	//to the local folder the resulting path is within one in one string and goes from root to the file
	std::string filepath = dir + '/' + std::string(path);

	GLuint texID;		//This is the ID that shall be returned

	glGenTextures(1, &texID);				//Generate 1 texture object and link it to texID

	//SOIL LOAD
	int width = 0;
	int height = 0;

	unsigned char* pic = SOIL_load_image(
		filepath.c_str(),					//Where soil shall load from
		&width,
		&height,
		0,									//number of channels
		SOIL_LOAD_RGBA						//What to load the image as (what if an image doesn't have an alpha component?)
	);

	glBindTexture(GL_TEXTURE_2D, texID);	//Bind the ID as an 2D texture

	//Load the image that was gotten via soil
	glTexImage2D(
		GL_TEXTURE_2D,
		0,								//This is how many mipmap levels may be created
		GL_RGBA,						//The type of color value for a pixel
		width,							//Width and height of the loaded image
		height,
		0,								//Ah the border value. Thou art a strange thing
		GL_RGBA,						//The format a picture is read as
		GL_UNSIGNED_BYTE,				//The value type of the data that came out from SOIL
		pic								//The data that came out of soil
	);
	
	//Now set how wrapping will work
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	//Clamp S-axis to border. (s-axis = x-axis)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	//Clamp T-axis to border. (t-axis = y-axis)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);		//When shrunk go pixely
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);		//When enlarged go pixely
	glBindTexture(GL_TEXTURE_2D, 0);

	//Mipmap won't do anything unless the 0 is changed
	//glGenerateMipmap(GL_TEXTURE_2D);

	//Free the data that soil holds. It's saved within the texture currently
	SOIL_free_image_data(pic);

	return(texID);
}

//Public
JMmodel::JMmodel(GLchar* path) {
	this->loadModel(path);
}

void JMmodel::drawModel(JMshader* shaderPtr) {
	//Loop through the meshes and call their draw functions
	for (int i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].drawMesh(shaderPtr);
	}
}

