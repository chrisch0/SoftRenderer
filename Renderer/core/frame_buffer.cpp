#include <cassert>
#include <exception>
#include <string>
#include <iostream>
#include <algorithm>
#include "frame_buffer.h"

FrameBuffer::FrameBuffer(int width, int height, int channels) :
	m_width(width), m_height(height), m_channels(channels)
{
	assert(m_width > 0 && m_height > 0);
	m_bufferSize = m_width * m_height * m_channels;

	try
	{
		m_buffer = new unsigned char[m_bufferSize]();
		//std::memset(m_buffer, 0, m_bufferSize);
	}
	catch (const std::bad_alloc& e)
	{
		std::string msg = "Bad allocate!\nImage buffer allocate fail!\n" + std::string(e.what());
		std::cerr << msg << std::endl;
	}
}

FrameBuffer::~FrameBuffer()
{
	delete[](m_buffer);
}

FrameBuffer::FrameBuffer(const FrameBuffer& rhs) :
	m_width(rhs.m_width), m_height(rhs.m_height), m_bufferSize(rhs.m_bufferSize)
{
	m_buffer = new unsigned char[m_bufferSize];
	std::memcpy(m_buffer, rhs.m_buffer, m_bufferSize);
}

FrameBuffer& FrameBuffer::operator=(const FrameBuffer& rhs)
{
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_bufferSize = rhs.m_bufferSize;

	unsigned char* new_buffer = new unsigned char[m_bufferSize];
	std::memcpy(new_buffer, rhs.m_buffer, m_bufferSize);
	delete[] m_buffer;
	m_buffer = new_buffer;
	return *this;
}

void FrameBuffer::SetColorRGB(int x, int y, Color color)
{
	assert(x < m_width && y < m_height);
	//int flipped_row = m_height - y - 1;
	//int index = flipped_row * m_width * m_channels + x * m_channels;
	int index = m_width * y * m_channels + x * m_channels;
	m_buffer[index] = std::clamp(color.x, 0.0f, 1.0f) * 255;
	m_buffer[index + 1] = std::clamp(color.y, 0.0f, 1.0f) * 255;
	m_buffer[index + 2] = std::clamp(color.z, 0.0f, 1.0f) * 255;
}

void FrameBuffer::SetColorBGR(int x, int y, Color color)
{
	assert(x < m_width && y < m_height);
	//int flipped_row = m_height - y - 1;
	//int index = flipped_row * m_width * m_channels + x * m_channels;
	int index = m_width * y * m_channels + x * m_channels;
	m_buffer[index] = std::clamp(color.z, 0.0f, 1.0f) * 255;
	m_buffer[index + 1] = std::clamp(color.y, 0.0f, 1.0f) * 255;
	m_buffer[index + 2] = std::clamp(color.x, 0.0f, 1.0f) * 255;
	m_buffer[index + 3] = std::clamp(color.w, 0.0f, 1.0f) * 255;
}

void FrameBuffer::SetBuffer(unsigned char* buffer)
{
	m_buffer = buffer;
}

void FrameBuffer::Clear(Color color)
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