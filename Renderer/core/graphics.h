#pragma once
#include <functional>
#include "math/math.h"

struct VSInput
{
	float3 position;
	float2 uv;
	float3 normal;
	Color color;
};

struct PSInput
{
	float4 sv_position;
	float3 normal;
	float3 positionWS;
	float2 uv;
	float4 color;
};

struct ConstantBuffer
{
	float4 resolution;
	float2 time;
	float4 mouse;
	float4x4 viewMat;
	float4x4 projMat;
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

