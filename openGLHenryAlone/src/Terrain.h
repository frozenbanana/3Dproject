#pragma once
#include <string>
#include <vector>
#include "Mesh.h"

struct BMPData
{
	unsigned char header[52];
	unsigned int dataPos;		// Position in file where data starts
	unsigned int width, height;
	unsigned int imageSize;		// bmpWidth * bmpHeight * 3				
	unsigned char* data;		// rgb data
	bool good;
};

//
//class HeightMap
//{
//public:
//	HeightMap(std::string fileName);

//private:
//	void LoadBMPData(std::string fileName);
//	Vertex *vertices;
//	Vertex m_numVerticies;
//	unsigned int* m_indices;
//	unsigned int m_numIndices;

//};

class Terrain
{
public:
	Terrain(std::string fileName, unsigned int maxHeight = 10);
	void Draw();
	Mesh& GetMesh();
	~Terrain();
private:
	BMPData m_BMPData;
	Mesh* m_terrainMeshPtr;

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	int m_width, m_height;
	unsigned int MAX_HEIGHT;

	bool computedNormals;
	bool computedIndices;

	void LoadBMPData(std::string fileName);
	void SetMeshData(BMPData BMPData);
	void SetHeight(unsigned zPos, unsigned xPos, float height);
	void ComputePos();
	void ComputeNormals();
	void ComputeTexCoords();
	void ComputeIndices();


};

