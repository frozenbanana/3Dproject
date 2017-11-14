#include "Terrain.h"
#include <iostream>


Terrain::Terrain(std::string fileName, unsigned int maxHeight)
{
	this->LoadBMPData(fileName);
	this->SetMeshData(this->m_BMPData);

	if (!this->m_vertices.empty()) {
		this->m_terrainMeshPtr = new Mesh(this->m_vertices.data, this->m_vertices.size(), this->m_indices.data, this->m_indices.size());
	}
}

void Terrain::SetMeshData(BMPData BMPData)
{
	if (BMPData.good)
	{
		this->m_width = BMPData.width;
		this->m_height = BMPData.height;
		this->m_vertices.resize(this->m_width * this->m_height);

		this->ComputePos();
		this->ComputeNormals();
		this->ComputeTexCoords();
		this->ComputeIndices();
	}
}

void Terrain::LoadBMPData(std::string fileName)
{
	// open file
	FILE* file = fopen(fileName.c_str(), "rb");
	if (!file)
	{
		std::cout << "Image could not be opened\n" << std::endl;
		this->m_BMPData.good = false;
		return;
	}
	if (fread(this->m_BMPData.header, 1, 54, file) != 54) // first 54 bytes has to be read before rgb data begins
	{
		std::cout << "Not a correct BMP file\n" << std::endl;
		this->m_BMPData.good = false;
		return;
	}
	if (this->m_BMPData.header[0] != 'B' || this->m_BMPData.header[1] != 'M')
	{
		std::cout << "File does not begin with 'BM'" << std::endl;
		this->m_BMPData.good = false;
		return;
	}

	// read ints fromm the byte array
	this->m_BMPData.dataPos = *(int*)&(this->m_BMPData.header[0x0A]);		// looking at a hexeditor on pos a it should have the vale 0x36 = 54
	this->m_BMPData.imageSize = *(int*)&(this->m_BMPData.header[0x22]);
	this->m_BMPData.width = *(int*)&(this->m_BMPData.header[0x12]);
	this->m_BMPData.height = *(int*)&(this->m_BMPData.header[0x16]);

	// checks if file is not correctly formated
	if (this->m_BMPData.imageSize == 0)
		this->m_BMPData.imageSize = this->m_BMPData.height*this->m_BMPData.width * 3;

	if (this->m_BMPData.dataPos == 0)
		this->m_BMPData.dataPos = 54;

	this->m_BMPData.data = new unsigned char[this->m_BMPData.imageSize];

	fread(this->m_BMPData.data, 1, this->m_BMPData.imageSize, file);
	
	this->m_BMPData.good = true;

	std::cout << "Height map loading.. " << std::endl;
	std::cout << "Name: " << fileName << std::endl;
	std::cout << "Width: " << m_BMPData.width << std::endl;
	std::cout << "Height: " << m_BMPData.height << std::endl;
	std::cout << "The pos where image data begins: " << m_BMPData.dataPos << std::endl;
	fclose(file);
}

void Terrain::ComputeIndices()
{
	for (GLuint h = 0; h < this->m_height-1; h++)// needs to be height - 1 to avoid make triangle when the last in row is finished
	{
		for (GLuint w = 0; w < this->m_width-1; w++) // needs to be width - 1 to avoid make triangle when the last in row is finished
		{
			// indicies
			unsigned int offset = (h * this->m_width + w);
			
			// triangle 1
			this->m_indices.push_back(offset);
			this->m_indices.push_back(offset + 1);
			this->m_indices.push_back(offset + this->m_width);
			
			// triangle 2
			this->m_indices.push_back(offset + 1);
			this->m_indices.push_back(offset + 1 + this->m_width);
			this->m_indices.push_back(offset + this->m_width);
		}
	}
}

void Terrain::ComputePos()
{
	for (size_t h = 0; h < this->m_height; h++)
	{
		for (size_t w = 0; w < this->m_width; w++)
		{
			// only need one color value to calc height, multiply by 3
			unsigned char color = (float)this->m_BMPData.data[3 * (h * this->m_width + w)];

			// convert to 0-1 float and half range
			float currentHeightPerPixel = this->MAX_HEIGHT *((color / 255.0f) - 0.5f);
			// set height
			this->SetHeight(h, w, currentHeightPerPixel);
		}
	}
}

void Terrain::ComputeNormals()
{
	if (this->computedNormals) {
		return;
	}

	//Compute the rough version of the normals
	glm::vec3** normals2 = new glm::vec3*[this->m_height];
	for (size_t i = 0; i < this->m_height; i++) {
		normals2[i] = new glm::vec3[this->m_width];
	}

	for (size_t z = 0; z < this->m_height; z++) {
		for (size_t x = 0; x < this->m_width; x++) {
			glm::vec3 sum(0.0f, 0.0f, 0.0f);

			glm::vec3 out;
			if (z > 0) {
				out = glm::vec3(0.0f, (this->m_vertices[(z - 1)*this->m_width + x].GetPos().y - this->m_vertices[z*this->m_width + x].GetPos().y, -1.0f));
			}
			glm::vec3 in;
			if (z < this->m_height - 1) {
					
				in = glm::vec3(0.0f, this->m_vertices[(z + 1)*this->m_width + x].GetPos().y - this->m_vertices[z*this->m_width + x].GetPos().y, 1.0f);
			}
			glm::vec3 left;
			if (x > 0) {
				left = glm::vec3(-1.0f, this->m_vertices[z*this->m_width + x-1]->GetPos().y - this->m_vertices[z*this->m_width + x]->.GetPos().y, 0.0f); 
			}
			glm::vec3 right;
			if (x < this->m_width - 1) {
				right = glm::vec3(1.0f, this->m_vertices[z*this->m_width + x + 1]->GetPos().y - this->m_vertices[z*this->width + x]->GetPos().y, 0.0f); 
			}

			if (x > 0 && z > 0) {
				sum += glm::normalize(glm::cross(out, left));
			}
			if (x > 0 && z < this->m_height - 1) {
				sum += glm::normalize(glm::cross(left, in));
			}
			if (x < this->m_width - 1 && z < this->m_height - 1) {
				sum += glm::normalize(glm::cross(in, right));
			}
			if (x < this->m_width - 1 && z > 0) {
				sum += glm::normalize(glm::cross(right, out));
			}

			normals2[z][x] = glm::normalize(sum);

		}
	}

	//Smooth out the normals
	const float FALLOUT_RATIO = 0.5f;
	for (GLuint z = 0; z < this->m_height; z++) {
		for (GLuint x = 0; x < this->m_width; x++) {
			glm::vec3 sum = normals2[z][x];

			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < this->m_width - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < this->m_height - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}
			if (glm::length(sum) == 0) {
				sum = glm::vec3(0.0f, 1.0f, 0.0f);
			}

			this->m_vertices[z*this->m_width + x]->GetNormal() = glm::normalize(sum);
		}
	}

	for (GLuint i = 0; i < this->m_height; i++) {
		delete[] normals2[i];
	}
	delete[] normals2;

	this->computedNormals = true;
}

void Terrain::ComputeTexCoords()
{
	for (GLuint h = 0; h < this->m_height; h++)
	{
		for (GLuint w = 0; w < this->m_width; w++)
		{
			this->m_vertices[h * this->m_width + w]->GetTexCoord().x = (GLfloat)w / ((GLfloat)this->m_width - 1);
			this->m_vertices[h * this->m_width + w]->GetTexCoord().y = (GLfloat)h / ((GLfloat)this->m_height - 1);
		}
	}
}

void Terrain::ComputeIndices()
{
	for (GLuint h = 0; h < this->m_height-1; h++)// needs to be height - 1 to avoid make triangle when the last in row is finished
	{
		for (GLuint w = 0; w < this->m_width-1; w++) // needs to be width - 1 to avoid make triangle when the last in row is finished
		{
			// indicies
			GLuint offset = (h * this->m_width + w);
			// triangle 1
			this->m_indices.push_back(offset);
			this->m_indices.push_back(offset + 1);
			this->m_indices.push_back(offset + this->m_width);
			// trim_angle 2
			this->m_indices.push_back(offset + 1);
			this->m_indices.push_back(offset + 1 + this->m_width);
			this->m_indices.push_back(offset + this->m_width);
		}
	}
}

void Terrain::ComputeNormals()
{
	if (this->computedNormals) {
		return;
	}

	//Compute the rough version of the normals
	glm::vec3** normals2 = new glm::vec3*[this->m_height];
	for (size_t i = 0; i < this->m_height; i++) {
		normals2[i] = new glm::vec3[this->m_width];
	}

	for (size_t z = 0; z < this->m_height; z++) {
		for (size_t x = 0; x < this->m_width; x++) {
			glm::vec3 sum(0.0f, 0.0f, 0.0f);

			glm::vec3 out;
			if (z > 0) {
				out = glm::vec3(0.0f, (this->m_vertices[(z - 1)*this->m_width + x]->GetPos()).y - this->vertices[z*this->m_width + x]->GetPos().y, -1.0f);
			}
			glm::vec3 in;
			if (z < this->m_height - 1) {
					
				in = glm::vec3(0.0f, this->m_vertices[(z + 1)*this->m_width + x]->GetPos().y - this->vertices[z*this->m_width + x]->GetPos().y, 1.0f);
			}
			glm::vec3 left;
			if (x > 0) {
				left = glm::vec3(-1.0f, this->m_vertices[z*this->m_width + x-1]->GetPos().y - this->vertices[z*this->m_width + x]->GetPos().y, 0.0f); 
			}
			glm::vec3 right;
			if (x < this->m_width - 1) {
				right = glm::vec3(1.0f, this->m_vertices[z*this->m_width + x + 1]->GetPos().y - this->vertices[z*this->m_width + x]->GetPos().y, 0.0f); 
			}

			if (x > 0 && z > 0) {
				sum += glm::normalize(glm::cross(out, left));
			}
			if (x > 0 && z < this->m_height - 1) {
				sum += glm::normalize(glm::cross(left, in));
			}
			if (x < this->m_width - 1 && z < this->m_height - 1) {
				sum += glm::normalize(glm::cross(in, right));
			}
			if (x < this->m_width - 1 && z > 0) {
				sum += glm::normalize(glm::cross(right, out));
			}

			normals2[z][x] = glm::normalize(sum);

		}
	}

	//Smooth out the normals
	const float FALLOUT_RATIO = 0.5f;
	for (GLuint z = 0; z < this->m_height; z++) {
		for (GLuint x = 0; x < this->m_width; x++) {
			glm::vec3 sum = normals2[z][x];

			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < this->m_width - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < this->m_height - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}
			if (glm::length(sum) == 0) {
				sum = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			this->m_vertices[z*this->m_width + x]->GetNormal() = glm::normalize(sum);
		}
	}

	for (GLuint i = 0; i < this->m_height; i++) {
		delete[] normals2[i];
	}
	delete[] normals2;

	this->computedNormals = true;
}

void Terrain::ComputeTexCoords()
{
	for (GLuint h = 0; h < this->m_height; h++)
	{
		for (GLuint w = 0; w < this->m_width; w++) 
		{
			this->m_vertices[h * this->m_width + w]->GetTexCoord().x = (GLfloat)w / ((GLfloat)this->m_width-1);
			this->m_vertices[h * this->m_width + w]->GetTexCoord().y = (GLfloat)h / ((GLfloat)this->m_height-1);
		}
	}
}

void Terrain::SetHeight(GLuint zPos, GLuint xPos, GLfloat height)
{ 
	this->m_vertices[zPos*this->m_width + xPos]->GetPos().x = (GLfloat)xPos;
	this->m_vertices[zPos*this->m_width + xPos]->GetPos().y = height;
	this->m_vertices[zPos*this->m_width + xPos]->GetPos().z = (GLfloat)zPos;
	this->computedNormals = false;
 };

void Terrain::Draw()
{
	this->m_terrainMeshPtr->Draw();
}

Terrain::~Terrain()
{
	delete this->m_terrainMeshPtr;
}
