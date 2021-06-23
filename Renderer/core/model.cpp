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

	for (auto& p : m_pMaterials)
	{
		if (p.second != nullptr)
		{
			delete p.second;
		}
	}
}

void GetVertexInfo(const std::string& dataBuffer, size_t& idx, size_t end, std::vector<float3>& positions, std::vector<float3>& colors, std::vector<float2>& texCoords, std::vector<float3>& normals);
void GetFaceInfo(const std::string& dataBuffer, size_t& idx, size_t end, const std::string& filename, const std::string& meshName, Mesh*& pCurMesh, std::unordered_map<std::string, Mesh*>& meshMap, size_t& modelIndexCount, size_t numPositions, size_t numTexCoords, size_t numNormals);

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
		Material* cur_mat = nullptr;
		auto model_name_start = filename.find_last_of('/') + 1;
		auto model_name_end = filename.find_last_of('.');
		std::string model_name = filename.substr(model_name_start, model_name_end - model_name_start);
		std::string mesh_name = model_name;
		std::string path = filename.substr(0, model_name_start);

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

			// set group material
			case 'u':
			{
				auto cmd_end = iter;
				while (cmd_end < line_end && data_buffer[cmd_end] != ' ')
					cmd_end++;
				std::string cmd(&data_buffer[iter], cmd_end - iter);
				if (cmd == "usemtl")
				{
					iter = cmd_end;
					SkipSpaces(data_buffer, iter);
					//SetMaterial(data_buffer, iter, line_end, m_pMaterials, cur_mesh);
				}
			}
			break;

			// get material info
			case 'm':
			{
				auto cmd_end = iter;
				while (cmd_end < line_end && data_buffer[cmd_end] != ' ')
					cmd_end++;
				std::string cmd(&data_buffer[iter], cmd_end - iter);
				if (cmd == "mtllib")
				{
					iter = cmd_end;
					SkipSpaces(data_buffer, iter);
					
				}
			}
			break;

			}

			line_beg = 1 + (IsLineEnd(data_buffer[line_beg]) ? line_beg : line_end);
			line_end = FindLineEnd(data_buffer, line_beg);
		}

		// build mesh
		BuildModel(positions, colors, texture_coords, normals);

	}
}

void Model::BuildModel(std::vector<float3>& positions, std::vector<float3>& colors, std::vector<float2>& texture_coords, std::vector<float3>& normals)
{
	m_indexBuffer.resize(m_indexCount);
	m_vertexBuffer.resize(m_indexCount);
	bool has_color_info = positions.size() == colors.size();
	bool has_tex_coord_info = texture_coords.size() > 0;
	size_t cur_index_loc = 0;
	size_t cur_vertex_loc = 0;
	for (auto& mesh_pair : m_pMeshes)
	{
		Mesh* pMesh = mesh_pair.second;
		pMesh->IndexStartLocation = cur_index_loc;
		pMesh->VertexStartLocation = cur_vertex_loc;
		
		for (Face* pFace : pMesh->pFaces)
		{
			auto vertex_start_loc = cur_vertex_loc;
			for (int i = 0; i < 3; ++i)
			{
				Vertex& v = m_vertexBuffer[cur_vertex_loc];
				v.position = positions[pFace->Positions[i]];
				v.color = has_color_info ? positions[pFace->Positions[i]] : float4(1.0, 1.0, 1.0, 1.0);
				v.normal = normals[pFace->Normals[i]];
				v.uv = has_tex_coord_info ? texture_coords[pFace->TexCoords[i]] : float2(0.0, 0.0);

				m_indexBuffer[cur_index_loc] = cur_vertex_loc;

				cur_vertex_loc++;
				cur_index_loc++;

				pMesh->BBox.Min(v.position);
				pMesh->BBox.Max(v.position);
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

				pMesh->BBox.Min(v.position);
				pMesh->BBox.Max(v.position);
			}

		}
		m_bbox.Min(pMesh->BBox.BoxMin);
		m_bbox.Max(pMesh->BBox.BoxMax);
	}
}

void Model::Draw(GraphicsContext& context)
{
	context.SetVertexBuffer(m_vertexBuffer.data());
	context.SetIndexBuffer(m_indexBuffer.data());
	for (auto& mesh_iter : m_pMeshes)
	{
		Mesh* pMesh = mesh_iter.second;
		context.DrawIndexed(pMesh->IndexCount, pMesh->IndexStartLocation, pMesh->VertexStartLocation);
	}
}

float3 GetColorRGB(const std::string& dataBuffer, size_t& idx, size_t end)
{
	float3 color;
	color.x = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx);	SkipToken(dataBuffer, idx, end);
	color.y = std::atof(&dataBuffer[idx]); SkipSpaces(dataBuffer, idx); SkipToken(dataBuffer, idx, end);
	color.z = std::atof(&dataBuffer[idx]);
	
	return color;
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

void GetFaceInfo(const std::string& dataBuffer, size_t& idx, size_t end, const std::string& filename, const std::string& meshName, Mesh*& pCurMesh, std::unordered_map<std::string, Mesh*>& meshMap, size_t& modelIndexCount, size_t numPositions, size_t numTexCoords, size_t numNormals)
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
			int index = std::atoi(&dataBuffer[idx]);
			if (index < 0)
			{
				++step;
			}
			int tmp = index;
			while ((tmp = tmp / 10) != 0)
			{
				++step;
			}

			if (index > 0)
			{
				if (pos == 0)
					face->Positions.push_back(index - 1);
				else if (pos == 1)
					face->TexCoords.push_back(index - 1);
				else if (pos == 2)
					face->Normals.push_back(index - 1);
				else
					std::cout << "Error face in " << filename << std::endl;
			}
			else if (index < 0)
			{
				if (pos == 0)
					face->Positions.push_back(numPositions + index);
				else if (pos == 1)
					face->TexCoords.push_back(numTexCoords + index);
				else if (pos == 2)
					face->Normals.push_back(numNormals + index);
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

void SetMaterial(const std::string& dataBuffer, size_t& idx, size_t end, std::unordered_map<std::string, Material*>& matMap, Mesh* pCurMesh)
{
	auto mat_end = idx;
	while (mat_end < end && dataBuffer[mat_end] != ' ')
	{
		mat_end++;
	}

	// mat file name is empty
	if (mat_end == idx)
		return;

	// already add material
	std::string mat_name(&dataBuffer[idx], mat_end - idx);
	if (pCurMesh->pMat != nullptr && pCurMesh->pMat->Name == mat_name)
		return;

	auto mat_iter = matMap.find(mat_name);
	if (mat_iter == matMap.end())
	{
		std::cout << "fail to locate material" << std::endl;
	}
	else
	{
		pCurMesh->pMat = mat_iter->second;
	}
}

void GetMaterialLib(const std::string& dataBuffer, size_t& idx, size_t end, const std::string& path, std::unordered_map<std::string, Material*>& matMap, Material*& pCurMat)
{
	auto mat_end = idx;
	while (mat_end < end && dataBuffer[mat_end] != ' ')
	{
		mat_end++;
	}

	if (mat_end == idx)
	{
		std::cout << ".mtl file is empty" << std::endl;
		return;
	}

	std::string filename(&dataBuffer[idx], mat_end - idx);
	std::ifstream fs;
	fs.open(path + filename);
	if (!fs.is_open())
	{
		std::cout << "fail to open " << path + filename << std::endl;
		return;
	}

	std::string mat_data((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
	size_t line_beg = 0;
	size_t line_end = FindLineEnd(mat_data, line_beg);
	while (line_beg < mat_data.size())
	{
		size_t iter = line_beg;

		switch (mat_data[iter])
		{
		// get material color
		case 'k':
		case 'K':
		{
			++iter;
			if (mat_data[iter] == 'a')
			{
				++iter;
				pCurMat->Ambient = GetColorRGB(mat_data, iter, line_end);
			}
			else if (mat_data[iter] == 'd')
			{
				++iter;
				pCurMat->Diffuse = GetColorRGB(mat_data, iter, line_end);
			}
			else if (mat_data[iter] == 's')
			{
				++iter;
				pCurMat->Specular = GetColorRGB(mat_data, iter, line_end);
			}
			else if (mat_data[iter] == 'e')
			{
				++iter;
				pCurMat->Emissive = GetColorRGB(mat_data, iter, line_end);
			}
		}
		break;

		// get material transmission color
		case 'T':
		{
			++iter;
			if (mat_data[iter] == 'f')
			{
				++iter;
				pCurMat->Transparent = GetColorRGB(mat_data, iter, line_end);
			}
			else if (mat_data[iter] == 'r')
			{
				++iter;
				pCurMat->Transparent.w = 1.0f - std::atof(&mat_data[iter]);
			}
		}
		break;

		case 'd':
		{

		}
		break;

		case 'N':
		case 'n':
		{
			++iter;
			// get specular exponent
			if (mat_data[iter] == 's')
			{
				++iter;
				pCurMat->Shineness = std::atof(&mat_data[iter]);
			}
			// get index of refraction
			else if (mat_data[iter] == 'i')
			{
				++iter;
				pCurMat->IndexOfRefraction = std::atof(&mat_data[iter]);
			}
			// create new material
			else if (mat_data[iter] == 'e')
			{
				SkipToken(mat_data, iter, line_end);
				SkipSpaces(mat_data, iter);
				auto name_end = iter;
				SkipToken(mat_data, name_end, line_end);
				std::string mat_name(&mat_data[iter], name_end - line_end);
				if (mat_name != "")
				{
					auto mat_iter = matMap.find(mat_name);
					if (mat_iter == matMap.end())
					{
						Material* mat = new Material(mat_name);
						pCurMat = mat;
						matMap.insert({ mat_name, mat });
					}
					else
					{
						pCurMat = mat_iter->second;
					}
				}
			}
		}
		break;

		// Texture
		case 'm':
		case 'b':
		case 'r':
		{

		}
		break;
		}

		line_beg = 1 + (IsLineEnd(mat_data[line_beg]) ? line_beg : line_end);
		line_end = FindLineEnd(mat_data, line_beg);
	}
}