#include "model.h"
#include "utils/io_utils.h"
#include <fstream>
#include <iostream>

Material::~Material()
{
	if (pAlbedoMap != nullptr)
		delete pAlbedoMap;
	if (pSpecularMap != nullptr)
		delete pSpecularMap;
}

Mesh::~Mesh()
{
	if (pMat != nullptr) 
		delete pMat; 
	for (auto face : pFaces)
	{
		if (face != nullptr)
			delete face;
	}
}

Model::~Model()
{
	for (auto& p : m_pMeshes)
	{
		if (p.second != nullptr)
		{
			delete p.second;
		}
	}
}

void GetVertexInfo(const std::string& dataBuffer, size_t& idx, size_t end, std::vector<float3>& positions, std::vector<float3>& colors, std::vector<float2>& texCoords, std::vector<float3>& normals);
void GetFaceInfo(const std::string& dataBuffer, size_t& idx, size_t end, const std::string& filename, const std::string& meshName, Mesh* pCurMesh, std::unordered_map<std::string, Mesh*>& meshMap, size_t& modelIndexCount, size_t numPositions, size_t numTexCoords, size_t numNormals);

void Model::LoadFromOBJ(const std::string& filename)
{
	std::ifstream fs;
	fs.open(filename);
	if (!fs.is_open())
	{
		std::cout << "failed to open " << filename << "\n";
	}
	else
	{
		// read whole file
		std::string data_buffer((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
		fs.close();

		size_t line_beg = 0;
		size_t line_end = FindLineEnd(data_buffer, line_beg);

		std::vector<float3> positions;
		std::vector<float3> colors;
		std::vector<float2> texture_coords;
		std::vector<float3> normals;
		Mesh* cur_mesh = nullptr;
		auto model_name_start = filename.find_last_of('/') + 1;
		auto model_name_end = filename.find_last_of('.');
		std::string model_name = filename.substr(model_name_start, model_name_end - model_name_start);
		std::string mesh_name = model_name;

		while (line_beg < data_buffer.size())
		{
			size_t iter = line_beg;
			switch (data_buffer[iter])
			{
			// get vertex position, (color), (texture coordinate), and normal
			case 'v':
			{
				iter++;
				GetVertexInfo(data_buffer, iter, line_end, positions, colors, texture_coords, normals);
			}
			break;

			// get face
			case 'f':
			{
				++iter;
				GetFaceInfo(data_buffer, iter, line_end, filename, mesh_name, cur_mesh, m_pMeshes, m_indexCount, positions.size(), texture_coords.size(), normals.size());
			}
			break;

			// get group and build group
			case 'g':
			{
				++iter;
				SkipSpaces(data_buffer, iter);
				mesh_name = std::string(&data_buffer[iter]);
				auto mesh_iter = m_pMeshes.find(mesh_name);
				if (mesh_iter != m_pMeshes.end())
				{
					cur_mesh = mesh_iter->second;
				}
				else
				{
					cur_mesh = new Mesh(mesh_name);
					m_pMeshes.insert({ mesh_name, cur_mesh });
				}
			}
			break;

			}

			line_beg = 1 + (IsLineEnd(data_buffer[line_beg]) ? line_beg : line_end);
			line_end = FindLineEnd(data_buffer, line_beg);
		}

		// build mesh
		//BuildModel();

	}
}

void Model::BuildModel(std::vector<float3>& positions, std::vector<float3>& colors, std::vector<float2>& texture_coords, std::vector<float3>& normals)
{
	m_indexBuffer.resize(m_indexCount);
	m_vertexBuffer.resize(m_indexCount);
	bool has_color_info = positions.size() == colors.size();
	for (auto& mesh_iter : m_pMeshes)
	{
		Mesh* pMesh = mesh_iter.second;
		pMesh->IndexStartLocation = m_indexBuffer.size();
		pMesh->VertexStartLocation = m_vertexBuffer.size();
		size_t cur_index_loc = pMesh->IndexStartLocation;
		size_t cur_vertex_loc = pMesh->VertexStartLocation;
		for (Face* pFace : pMesh->pFaces)
		{
			auto vertex_start_loc = cur_vertex_loc;
			for (int i = 0; i < 3; ++i)
			{
				Vertex& v = m_vertexBuffer[cur_vertex_loc];
				v.position = positions[pFace->Positions[i]];
				v.color = has_color_info ? positions[pFace->Positions[i]] : float4(1.0, 1.0, 1.0, 1.0);
				v.normal = normals[pFace->Normals[i]];
				v.uv = texture_coords[pFace->TexCoords[i]];

				m_indexBuffer[cur_index_loc] = cur_vertex_loc;

				cur_vertex_loc++;
				cur_index_loc++;
			}

			for (int i = 3; i < pFace->Positions.size(); ++i)
			{
				Vertex& v = m_vertexBuffer[cur_vertex_loc];
				v.position = positions[pFace->Positions[i]];
				v.color = has_color_info ? positions[pFace->Positions[i]] : float4(1.0, 1.0, 1.0, 1.0);
				v.normal = normals[pFace->Normals[i]];
				v.uv = texture_coords[pFace->TexCoords[i]];
				cur_vertex_loc++;

				m_indexBuffer[cur_index_loc] = vertex_start_loc;
				cur_index_loc++;
				m_indexBuffer[cur_index_loc] = vertex_start_loc + i - 1;
				cur_index_loc++;
				m_indexBuffer[cur_index_loc] = vertex_start_loc + i;
				cur_index_loc++;
			}

		}
	}
}

void GetVertexInfo(const std::string& dataBuffer, size_t& idx, size_t end, std::vector<float3>& positions, std::vector<float3>& colors, std::vector<float2>& texCoords, std::vector<float3>& normals)
{
	// vertex position
	if (dataBuffer[idx] == ' ' || dataBuffer[idx] == '\t')
	{
		int num_components = CountNumricComponentsInLine(dataBuffer, idx, end);
		if (num_components == 3)
		{
			float x = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx);	SkipToken(dataBuffer, idx, end);
			float y = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float z = std::atof(&dataBuffer[idx]);
			positions.emplace_back(x, y, z);
		}
		else if (num_components == 4)
		{
			float x = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float y = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float z = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx);	SkipToken(dataBuffer, idx, end);
			float w = std::atof(&dataBuffer[idx]);
			positions.emplace_back(x / w, y / w, z / w);
		}
		else if (num_components == 6)
		{
			float x = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx);	SkipToken(dataBuffer, idx, end);
			float y = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float z = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float r = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float g = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			float b = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
			positions.emplace_back(x, y, z);
			colors.emplace_back(r, g, b);
		}
	}
	// vertex texture coordinate
	else if (dataBuffer[idx] == 't')
	{
		++idx;
		float u = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx);	SkipToken(dataBuffer, idx, end);
		float v = std::atof(&dataBuffer[idx]);
		texCoords.emplace_back(u, v);
	}
	// vertex normal
	else if (dataBuffer[idx] == 'n')
	{
		float x = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx);	SkipToken(dataBuffer, idx, end);
		float y = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
		float z = std::atof(&dataBuffer[idx]);
		normals.emplace_back(x, y, z);
	}
}

void GetFaceInfo(const std::string& dataBuffer, size_t& idx, size_t end, const std::string& filename, const std::string& meshName, Mesh* pCurMesh, std::unordered_map<std::string, Mesh*>& meshMap, size_t& modelIndexCount, size_t numPositions, size_t numTexCoords, size_t numNormals)
{
	int pos = 0;
	Face* face = new Face();
	while (idx < end)
	{
		int step = 1;
		if (dataBuffer[idx] == '/')
		{
			pos++;
		}
		else if (dataBuffer[idx] == ' ')
		{
			pos = 0;
		}
		else
		{
			int idx = std::atoi(&dataBuffer[idx]);
			if (idx < 0)
			{
				++step;
			}
			int tmp = idx;
			while ((tmp = tmp / 10) != 0)
			{
				++step;
			}

			if (idx > 0)
			{
				if (pos == 0)
					face->Positions.push_back(idx - 1);
				else if (pos == 1)
					face->TexCoords.push_back(idx - 1);
				else if (pos == 2)
					face->Normals.push_back(idx - 1);
				else
					std::cout << "Error face in " << filename << std::endl;
			}
			else if (idx < 0)
			{
				if (pos == 0)
					face->Positions.push_back(numPositions + idx);
				else if (pos == 1)
					face->TexCoords.push_back(numTexCoords + idx);
				else if (pos == 2)
					face->Normals.push_back(numNormals + idx);
				else
					std::cout << "Error face in " << filename << std::endl;
			}
			else
			{
				delete face;
				std::cout << "Invalid face indices in " << filename << std::endl;
			}
		}
		idx += step;
	}
	if (face->Positions.empty())
	{
		std::cout << "Empty face in " << filename << std::endl;
		delete face;
	}
	else
	{
		if (pCurMesh == nullptr)
		{
			pCurMesh = new Mesh(meshName);
			meshMap.insert({ meshName, pCurMesh });
		}
		pCurMesh->pFaces.push_back(face);
		pCurMesh->IndexCount += (face->Positions.size() - 2) * 3;
		modelIndexCount += (face->Positions.size() - 2) * 3;
	}
}

