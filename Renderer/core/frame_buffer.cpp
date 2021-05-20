#include <cassert>
#include <exception>
#include <string>
#include <iostream>
#include "frame_buffer.h"

FrameBuffer::FrameBuffer(int width, int height) :
	m_width(width), m_height(height)
{
	assert(m_width > 0 && m_height > 0);
	m_bufferSize = m_width * m_height * 4;

	try
	{
		m_buffer = new unsigned char[m_bufferSize]();
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
	int flipped_row = m_height - y - 1;
	int index = flipped_row * m_width * 4 + x * 4;
	m_buffer[index] = color.x;
	m_buffer[index + 1] = color.y;
	m_buffer[index + 2] = color.z;
}

void FrameBuffer::SetColorBGR(int x, int y, Color color)
{
	assert(x < m_width && y < m_height);
	int flipped_row = m_height - y - 1;
	int index = flipped_row * m_width * 4 + x * 4;
	m_buffer[index] = color.z;
	m_buffer[index + 1] = color.y;
	m_buffer[index + 2] = color.x;
}

void FrameBuffer::SetBuffer(unsigned char* buffer)
{
	m_buffer = buffer;
}