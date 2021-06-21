#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "math/math.h"
#include "graphics.h"

class Texture;

struct Face
{
	std::vector<uint32_t> Positions;
	std::vector<uint32_t> TexCoords;
	std::vector<uint32_t> Normals;
};

struct Material
{
	Material(const std::string& name) : Name(name), pAlbedoMap(nullptr), pSpecularMap(nullptr) {}
	Material() : Name("Default"), pAlbedoMap(nullptr), pSpecularMap(nullptr) {}
	~Material();
	std::string Name;
	Texture* pAlbedoMap;
	Texture* pSpecularMap;

};

struct Mesh
{
	Mesh() : pMat(nullptr), IndexCount(0), Name("Default") {	}
	Mesh(const std::string& name) : pMat(nullptr), IndexCount(0), Name(name) {	}
	~Mesh();
	std::string Name;
	Material* pMat;
	std::vector<Face*> pFaces;
	size_t VertexStartLocation;
	size_t IndexStartLocation;
	uint32_t IndexCount;
	BoundingBox3D BBox;
};

class Model
{
public:
	Model() {}
	~Model();
	void LoadFromOBJ(const std::string& filename);
	void Draw(GraphicsContext& context);
private:
	void BuildModel(std::vector<float3>& positions,
		std::vector<float3>& colors,
		std::vector<float2>& texture_coords,
		std::vector<float3>& normals);
	std::unordered_map<std::string, Mesh*> m_pMeshes;
	std::vector<Vertex> m_vertexBuffer;
	std::vector<uint32_t> m_indexBuffer;
	BoundingBox3D bbox;
	uint32_t m_indexCount;
};