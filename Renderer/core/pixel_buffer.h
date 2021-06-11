#pragma once
#include "math/vec.h"
#include <cassert>
#include <exception>
#include <string>
#include <iostream>
#include <algorithm>
#include <type_traits>

template <typename T, int NumChannels, bool AllocateMem>
class PixelBuffer
{
public:
	PixelBuffer(int width, int height /*, int channels = 4, bool allocateMem = true */)
		: m_width(width), m_height(height)
	{
		assert(m_width > 0 && m_height > 0);
		m_bufferSize = m_width * m_height * NumChannels;

		if (AllocateMem)
		{
			try
			{
				m_buffer = new T[m_bufferSize]();
				//std::memset(m_buffer, 0, m_bufferSize);
			}
			catch (const std::bad_alloc& e)
			{
				std::string msg = "Bad allocate!\nImage buffer allocate fail!\n" + std::string(e.what());
				std::cerr << msg << std::endl;
			}
		}
	}

	~PixelBuffer()
	{
		if (AllocateMem)
			delete[](m_buffer);
	}

	PixelBuffer(const PixelBuffer&) = delete;
	PixelBuffer& operator=(const PixelBuffer& rhs) = delete;

	unsigned char* GetBuffer() { return m_buffer; }
	const int GetWidth() const { return m_width; }
	const int GetHeight() const { return m_height; }
	const size_t GetBufferSize() const { return m_bufferSize; }
	void SetWidth(int width) { m_width = width; }
	void SetHeight(int height) { m_height = height; }

	T GetValue(size_t idx)
	{
		assert(idx < m_bufferSize);
		return m_buffer[idx];
	}

	T GetValue(int x, int y)
	{
		int index = m_width * y * NumChannels + x * NumChannels;
		return m_buffer[index];
	}

	void SetValue(size_t idx, T value)
	{
		assert(idx < m_bufferSize);
		m_buffer[idx] = value;
	}

	void SetValue(int x, int y, T value)
	{
		int idx = m_width * y * NumChannels + x * NumChannels;
		assert(idx < m_bufferSize);
		m_buffer[idx] = value;
	}
	void SetColorRGB(int x, int y, Color color)
	{
		assert(x < m_width && y < m_height);
		//int flipped_row = m_height - y - 1;
		//int index = flipped_row * m_width * m_channels + x * m_channels;
		int index = m_width * y * NumChannels + x * NumChannels;
		m_buffer[index] = std::clamp(color.x, 0.0f, 1.0f) * 255;
		m_buffer[index + 1] = std::clamp(color.y, 0.0f, 1.0f) * 255;
		m_buffer[index + 2] = std::clamp(color.z, 0.0f, 1.0f) * 255;
	}
	void SetColorBGR(int x, int y, Color color)
	{
		assert(x < m_width && y < m_height);
		//int flipped_row = m_height - y - 1;
		//int index = flipped_row * m_width * m_channels + x * m_channels;
		int index = m_width * y * NumChannels + x * NumChannels;
		m_buffer[index] = std::clamp(color.z, 0.0f, 1.0f) * 255;
		m_buffer[index + 1] = std::clamp(color.y, 0.0f, 1.0f) * 255;
		m_buffer[index + 2] = std::clamp(color.x, 0.0f, 1.0f) * 255;
		m_buffer[index + 3] = std::clamp(color.w, 0.0f, 1.0f) * 255;
	}
	void SetBuffer(T* buffer) 
	{
		m_buffer = buffer;
	}

	void Clear(Color color)
	{
		{
#pragma omp parallel for schedule(dynamic)
			for (int x = 0; x < m_width; ++x)
			{
				for (int y = 0; y < m_height; ++y)
				{
					SetColorBGR(x, y, color);
				}
			}
		}
	}

	static constexpr int channels = NumChannels;

protected:
	int m_width;
	int m_height;
	//int m_channels;
	size_t m_bufferSize;
	T* m_buffer = nullptr;
	//bool m_allocated;
};

using FrameBuffer = PixelBuffer<unsigned char, 4, false>;
using ColorBuffer = PixelBuffer<float, 4, true>;
using DepthBuffer = PixelBuffer<float, 1, true>;
using StencilBuffer = PixelBuffer<unsigned char, 1, true>;

//struct DepthBuffer : public FrameBuffer
//{
//public:
//	DepthBuffer(int width, int height);
//	~DepthBuffer();
//	DepthBuffer(const DepthBuffer&);
//	DepthBuffer& operator=(const DepthBuffer& rhs);
//	void SetDepth(int x, int y, float depth, eDepthFunc depthFunc);
//	void Clear(float depth);
//};
//
//struct StencilBuffer : public FrameBuffer
//{
//
//};