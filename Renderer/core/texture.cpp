#include "texture.h"
#include "utils/io_utils.h"
#include <fstream>
#include <iostream>
#include <cassert>

void ImageFlipH(Texture* texture);
void ImageFlipV(Texture* texture);
float ResolveTexCoord(eAddressMode addressMode, float x);
float2 ResolveTexCoord(eAddressMode addressModeU, eAddressMode addressModeV, const float2& uv);
int Warp(int x, int dim);

Texture::Texture() : m_width(0), m_height(0), m_channels(0), m_buffer(nullptr), m_size(0)
{

}

Texture::Texture(const std::string& name) : m_name(name), m_width(0), m_height(0), m_channels(0), m_buffer(nullptr), m_size(0)
{

}

Texture::~Texture()
{
	if (m_buffer != nullptr)
	{
		delete[] m_buffer;
	}
}

void Texture::LoadFromTGA(const std::string& path)
{
	int width, height, channels;
	int is_rle, flip_h, origin_left_upper;
	std::ifstream fs;
	fs.open(path, std::ios::binary);
	if (!fs.is_open())
	{
		std::cout << "fail to open " << path << std::endl;
		return;
	}

	int depth, idlength, imgtype, imgdesc;
	// read tga header
	{
		char header[TGA_HEADER_SIZE];
		fs.read(header, TGA_HEADER_SIZE);

		width = header[12] | (header[13] << 8);
		height = header[14] | (header[15] << 8);
		assert(width > 0 && height > 0);
		depth = header[16];
		assert(depth == 8 || depth == 24 || depth == 32);
		channels = depth / 8;

		idlength = header[0];
		assert(idlength == 0);

		imgtype = header[2];
		assert(imgtype == 2 || imgtype == 3 || imgtype == 10 || imgtype == 11);
		is_rle = imgtype == 10 || imgtype == 11;

		imgdesc = header[17];
		flip_h = imgdesc & 0x10;
		origin_left_upper = imgdesc & 0x20;
	}

	Create(width, height, channels);

	if (is_rle)
	{
		int cur_size = 0;
		while (cur_size < m_size)
		{
			unsigned char header;
			header = fs.get();
			int rle_packet = header & 0x80;
			int num_pixels = (header & 0x7F) + 1;
			unsigned char pixel[4];
			int i, j;
			assert(cur_size + num_pixels * m_channels <= m_size);
			if (rle_packet) {                                   /* rle packet */
				for (j = 0; j < m_channels; j++) {
					pixel[j] = fs.get();
				}
				for (i = 0; i < num_pixels; i++) {
					for (j = 0; j < m_channels; j++) {
						m_buffer[cur_size++] = pixel[j];
					}
				}
			}
			else {                                            /* raw packet */
				for (i = 0; i < num_pixels; i++) {
					for (j = 0; j < m_channels; j++) {
						m_buffer[cur_size++] = fs.get();
					}
				}
			}
		}
	}
	else
	{
		fs.read((char*)m_buffer, m_size);
	}

	fs.close();

	if (flip_h)
	{
		ImageFlipH(this);
	}
	if (!origin_left_upper)
	{
		ImageFlipV(this);
	}
	if (m_channels >= 3)
	{
		int x, y;
		for (y = 0; y < m_height; ++y)
		{
			for (x = 0; x < m_width; ++x)
			{
				auto pixel = GetRawValue(x, y);
				std::swap(pixel.x, pixel.z);
				// bgr to rgb
				SetRawValue(x, y, pixel);
			}
		}
	}

}

void Texture::Create(int width, int height, int channels)
{
	if (m_buffer != nullptr)
		delete[] m_buffer;
	m_width = width;
	m_height = height;
	m_channels = channels;
	m_size = width * height * channels;
	m_buffer = new unsigned char[m_size]();
}

Vec4<unsigned char> Texture::GetRawValue(int x, int y)
{
	int index = (y * m_width + x) * m_channels;
	Vec4<unsigned char> raw;
	raw.x = m_buffer[index];
	raw.y = m_buffer[index + 1];
	raw.z = m_buffer[index + 2];
	raw.w = m_channels == 4 ? m_buffer[index + 3] : 255;
	return raw;
}

void Texture::SetRawValue(int x, int y, const Vec4<unsigned char>& raw)
{
	int index = (y * m_width + x) * m_channels;
	m_buffer[index] = raw.x;
	m_buffer[index + 1] = raw.y;
	m_buffer[index + 2] = raw.z;
	if (m_channels == 4)
		m_buffer[index + 3] = raw.w;
}

float4 Texture::GetColor(int x, int y)
{
	int index = (y * m_width + x) * m_channels;
	float4 col;
	col.x = (float)m_buffer[index] / (float)255;
	col.y = (float)m_buffer[index + 1] / (float)255;
	col.z = (float)m_buffer[index + 2] / (float)255;
	col.w = m_channels == 4 ? (float)m_buffer[index + 3] / (float)255 : 1.0f;
	return col;
}

void Texture::SetColor(int x, int y, const float4& col)
{
	int index = (y * m_width + x) * m_channels;
	m_buffer[index] = std::clamp(col.x, 0.0f, 1.0f) * 255;
	m_buffer[index + 1] = std::clamp(col.y, 0.0f, 1.0f) * 255;
	m_buffer[index + 2] = std::clamp(col.z, 0.0f, 1.0f) * 255;
	if (m_channels == 4)
		m_buffer[index + 3] = std::clamp(col.w, 0.0f, 1.0f) * 255;
}

Color Texture::SampleLevel(const SamplerState& sampler, const float2& uv, int level)
{
	float2 r_uv = ResolveTexCoord(sampler.AddressU, sampler.AddressV, uv);
	if (r_uv >= float2(0.0f, 0.0f) && r_uv <= float2(1.0f, 1.0f))
	{
		float2 xy = uv * float2(m_width, m_height);
		int x0 = Warp((int)(xy.x - 0.5f), m_width);
		int y0 = Warp((int)(xy.y - 0.5f), m_height);
		int x1 = Warp((int)(xy.x - 0.5f) + 1, m_width);
		int y1 = Warp((int)(xy.y - 0.5f) + 1, m_height);
		float alpha = std::fmod(xy.x - 0.5f, 1.0);
		float beta = std::fmod(xy.y - 0.5f, 1.0);
		Color c00 = GetColor(x0, y0);
		Color c01 = GetColor(x0, y1);
		Color c10 = GetColor(x1, y0);
		Color c11 = GetColor(x1, y1);
		Color c_t = c00 * (1.0f - alpha) + c10 * alpha;
		Color c_b = c01 * (1.0f - alpha) + c11 * alpha;
		return c_t * (1.0f - beta) + c_b * beta;
	}
	else
	{
		return sampler.BorderColor;
	}
}

void ImageFlipH(Texture* texture)
{
	int half_width = texture->GetWidth() / 2;
	int x, y, k;
	for (y = 0; y < texture->GetHeight(); y++) 
	{
		for (x = 0; x < half_width; x++) 
		{
			int flipped_x = texture->GetWidth() - 1 - x;
			auto pixel1 = texture->GetRawValue(x, y);
			auto pixel2 = texture->GetRawValue(flipped_x, y);
			for (k = 0; k < texture->GetChannels(); k++) 
			{
				texture->SetRawValue(x, y, pixel2);
				texture->SetRawValue(flipped_x, y, pixel1);
			}
			
		}
	}
}

void ImageFlipV(Texture* texture)
{
	int half_height = texture->GetHeight() / 2;
	int x, y, k;
	for (y = 0; y < half_height; y++) 
	{
		for (x = 0; x < texture->GetWidth(); x++) 
		{
			int flipped_y = texture->GetHeight() - 1 - y;
			auto pixel1 = texture->GetRawValue(x, y);
			auto pixel2 = texture->GetRawValue(x, flipped_y);
			for (k = 0; k < texture->GetChannels(); k++) 
			{
				texture->SetRawValue(x, y, pixel2);
				texture->SetRawValue(x, flipped_y, pixel1);
			}
		}
	}
}

float ResolveTexCoord(eAddressMode addressMode, float x)
{
	if (x >= 0.0f && x <= 1.0f)
		return x;

	float t;
	switch (addressMode)
	{
	case  Address_Mode_Border:
	{
		if (x > 1.0f || x < 0.0f)
			t = -1.0f;
	}
	break;
	case Address_Mode_Clamp:
	{
		t = std::clamp(x, 0.0f, 1.0f);
	}
	break;
	case Address_Mode_Mirror:
	{
		t = std::fmodf(x, 2.0f);
		t = t <= 1.0f ? t : 2.0f - t;
	}
	break;
	case Address_Mode_Mirror_once:
	{
		t = std::clamp(std::fabs(x), 0.0f, 1.0f);
	}
		break;
	case Address_Mode_Warp:
	{
		t = std::fmodf(x, 1.0f);
	}
		break;
	}
	return t;
}

float2 ResolveTexCoord(eAddressMode addressModeU, eAddressMode addressModeV, const float2& uv)
{
	float2 r_uv;
	r_uv.x = ResolveTexCoord(addressModeU, uv.x);
	r_uv.y = ResolveTexCoord(addressModeV, uv.y);
	return r_uv;
}

int Warp(int x, int dim)
{
	if (x >= 0 && x < dim)
		return x;
	x = x % dim;
	return x > 0 ? x : dim + x;
}