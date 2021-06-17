#include "model.h"
#include "utils/io_utils.h"
#include <fstream>
#include <iostream>

Mesh::~Mesh()
{
	if (Mat != nullptr) 
		delete Mat; 
	for (auto face : Faces)
	{
		if (face != nullptr)
			delete face;
	}
}

void Model::LoadFromOBJ(const std::string& filename)
{
	std::ofstream test;
	test.open("test.txt", std::ios::out);
	std::ifstream fs;
	fs.open(filename);
	if (!fs.is_open())
	{
		std::cout << "failed to open " << filename << "\n";
	}
	else
	{
		// read whole file
		fs.seekg(0, std::ios::end);
		auto file_size = fs.tellg();
		fs.seekg(0, std::ios::beg);
		std::vector<char> data_buffer(file_size);
		fs.read(data_buffer.data(), file_size);
		fs.close();

		size_t line_beg = 0;
		size_t line_end = FindLineEnd(data_buffer, line_beg);

		std::vector<float3> vertices;
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
			case 'v':
			{
				iter++;
				// vertex position
				if (data_buffer[iter] == ' ' || data_buffer[iter] == '\t')
				{
					int num_components = CountNumricComponentsInLine(data_buffer, iter, line_end);
					if (num_components == 3)
					{
						float x = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter);	SkipToken(data_buffer, iter, line_end);
						float y = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float z = std::atof(&data_buffer[iter]);
						vertices.emplace_back( x, y, z );
					}
					else if (num_components == 4)
					{
						float x = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float y = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float z = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter);	SkipToken(data_buffer, iter, line_end);
						float w = std::atof(&data_buffer[iter]);
						vertices.emplace_back(x / w, y / w, z / w);
					}
					else if (num_components == 6)
					{
						float x = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter);	SkipToken(data_buffer, iter, line_end);
						float y = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float z = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float r = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float g = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						float b = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
						vertices.emplace_back(x, y, z);
						colors.emplace_back(r, g, b);
					}
				}
				// vertex texture coordinate
				else if (data_buffer[iter] == 't')
				{
					++iter;
					float u = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter);	SkipToken(data_buffer, iter, line_end);
					float v = std::atof(&data_buffer[iter]);
					texture_coords.emplace_back(u, v);
				}
				// vertex normal
				else if (data_buffer[iter] == 'n')
				{
					float x = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter);	SkipToken(data_buffer, iter, line_end);
					float y = std::atof(&data_buffer[iter]); SkipSpaces(data_buffer, iter); SkipToken(data_buffer, iter, line_end);
					float z = std::atof(&data_buffer[iter]);
					normals.emplace_back(x, y, z);
				}
			}
			break;

			case 'f':
			{
				++iter;
				int pos = 0;
				Face* face = new Face();
				while (iter < line_end)
				{
					int step = 1;
					if (data_buffer[iter] == '/')
					{
						pos++;
					}
					else if (data_buffer[iter] == ' ')
					{
						pos = 0;
					}
					else
					{
						int idx = std::atoi(&data_buffer[iter]);
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
								face->Vertices.push_back(idx - 1);
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
								face->Vertices.push_back(vertices.size() + idx);
							else if (pos == 1)
								face->TexCoords.push_back(texture_coords.size() + idx);
							else if (pos == 2)
								face->Normals.push_back(normals.size() + idx);
							else 
								std::cout << "Error face in " << filename << std::endl;
						}
						else
						{
							delete face;
							std::cout << "Invalid face indices in " << filename << std::endl;
						}
					}
					iter += step;
				}
				if (face->Vertices.empty())
				{
					std::cout << "Empty face in " << filename << std::endl;
					delete face;
				}
				else
				{
					if (cur_mesh == nullptr)
					{
						cur_mesh = new Mesh(mesh_name);
						m_meshes.insert({ mesh_name, cur_mesh });
					}
					cur_mesh->Faces.push_back(face);
					cur_mesh->IndexCount += face->Vertices.size() == 3 ? 3 : 6;
					test << "f " <<
						face->Vertices[0] + 1 << "/" << face->TexCoords[0] + 1 << "/" << face->Normals[0] + 1 << " " <<
						face->Vertices[1] + 1 << "/" << face->TexCoords[1] + 1 << "/" << face->Normals[1] + 1 << " " <<
						face->Vertices[2] + 1 << "/" << face->TexCoords[2] + 1 << "/" << face->Normals[2] + 1 << " \n";
				}
			}
			break;

			case 'g':
			{
				++iter;
				SkipSpaces(data_buffer, iter);
				mesh_name = std::string(&data_buffer[iter]);
				auto mesh_iter = m_meshes.find(mesh_name);
				if (mesh_iter != m_meshes.end())
				{
					cur_mesh = mesh_iter->second;
				}
				else
				{
					cur_mesh = new Mesh(mesh_name);
					m_meshes.insert({ mesh_name, cur_mesh });
				}
			}
			break;

			}

			auto tmp_beg = line_beg;
			auto tmp_end = line_end;
			line_beg = 1 + (IsLineEnd(data_buffer[line_beg]) ? line_beg : line_end);
			line_end = FindLineEnd(data_buffer, line_beg);
			if (tmp_beg == line_beg || tmp_end == line_end)
				std::cout << tmp_beg << " " << line_beg << "\n";
		}
		std::cout << cur_mesh->Faces.size() << std::endl;
		test.close();
	}
}