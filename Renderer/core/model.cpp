#include "model.h"
#include "utils/io_utils.h"
#include "math/math.h"
#include <fstream>
#include <iostream>

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
					int num_components = CountComponentsInLine(data_buffer, iter, line_end);
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

			}
			break;

			}

			line_beg = 1 + (IsLineEnd(data_buffer[line_beg]) ? line_beg : line_end);
			line_end = FindLineEnd(data_buffer, line_beg);
		}
	}
}