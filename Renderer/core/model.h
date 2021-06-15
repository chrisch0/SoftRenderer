#pragma once
#include <vector>
#include <string>

struct Mesh
{

};

class Model
{
public:
	void LoadFromOBJ(const std::string& filename);
private:
	std::vector<Mesh> m_meshes;
};