#pragma once
#include <string>
#include "math/math.h"
#include "Sampler.h"

class Texture
{
public:
	Texture();	
	Texture(const std::string& name);
	~Texture();
	void LoadFromTGA(const std::string& path);
	void Create(int width, int height, int channels);
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetChannels() const { return m_channels; }
	size_t GetSize() const { return m_size; }
	Vec4<unsigned char> GetRawValue(int x, int y);
	void SetRawValue(int x, int y, const Vec4<unsigned char>& raw);
	float4 GetColor(int x, int y);
	void SetColor(int x, int y, const float4& col);

	Color SampleLevel(const SamplerState& sampler, const float2& uv, int level = 0);
private:
	std::string m_name;
	int m_width;
	int m_height;
	int m_channels;
	size_t m_size;
	unsigned char* m_buffer;
};