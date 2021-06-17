#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "math/math.h"

class Texture;

struct Face
{
	std::vector<uint32_t> Vertices;
	std::vector<uint32_t> TexCoords;
	std::vector<uint32_t> Normals;
};

struct Material
{
	std::string Name;
	Texture* AlbedoMap;
	Texture* SpecularMap;

};

struct BoundingBox
{
	float3 Min;
	float3 Max;
};

struct Mesh
{
	Mesh() : Mat(nullptr), VertexCount(0), IndexCount(0), Name("Default") {	}
	Mesh(const std::string& name) : Mat(nullptr), VertexCount(0), IndexCount(0), Name(name) {	}
	~Mesh();
	std::string Name;
	Material* Mat;
	std::vector<Face*> Faces;
	uint32_t VertexCount;
	uint32_t IndexCount;
};

class Model
{
public:
	void LoadFromOBJ(const std::string& filename);
private:
	std::unordered_map<std::string, Mesh*> m_meshes;
};