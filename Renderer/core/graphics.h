#pragma once
#include <functional>
#include "math/math.h"

struct VSInput
{
	vec3f position;
	vec2f uv;
	vec3f normal;
	Color color;
};

struct PSInput
{
	vec4f sv_position;
	vec3f normal;
	vec3f positionWS;
	vec2f uv;
	vec4f color;
};

struct ConstantBuffer
{
	vec4f resolution;
	vec2f time;
	vec4f mouse;
};

using VSOut = PSInput;
using Vertex = VSInput;

using VertexShader = std::function<VSOut(VSInput*, void*)>;
using PixelShader = std::function<Color(PSInput*, void*)>;

struct Viewport
{
	float topLeftX;
	float topLeftY;
	float width;
	float height;
	float minDepth;
	float maxDepth;
};

struct RasterizerState
{

};

struct PipelineState
{
	VertexShader vertexShader;
	PixelShader pixelShader;
	
	// for clipping
	//VSOut inVertexAttribute[10];
	//VSOut outVertexAttribute[10];
};

