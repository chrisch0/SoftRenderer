#pragma once
#include "math/vec.h"

struct FrameBuffer
{
public:
	FrameBuffer(int width, int height, int channels = 4);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&);
	FrameBuffer& operator=(const FrameBuffer& rhs);

	unsigned char* GetBuffer() const { return m_buffer; }
	const int GetWidth() const { return m_width; }
	const int GetHeight() const { return m_height; }
	const size_t GetBufferSize() const { return m_bufferSize; }

	void SetColorRGB(int x, int y, Color color);
	void SetColorBGR(int x, int y, Color color);
	void SetBuffer(unsigned char* buffer);
	void Clear(Color color);

protected:
	int m_width;
	int m_height;
	int m_channels;
	size_t m_bufferSize;
	unsigned char* m_buffer;
};

struct DepthBuffer : public FrameBuffer
{
public:
	DepthBuffer(int width, int height);
	~DepthBuffer();
	DepthBuffer(const DepthBuffer&);
	DepthBuffer& operator=(const DepthBuffer& rhs);
	void SetDepth(int x, int y, float depth);

};