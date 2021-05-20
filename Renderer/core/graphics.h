#pragma once
#include <functional>
#include "math/vec.h"

struct VSInput
{
	vec3f position;
	vec2f uv;
	vec3f normal;
};

struct PSInput
{
	vec4f sv_position;
	vec3f normal;
	vec3f positionWS;
	vec2f uv;
};

using VSOut = PSInput;
using Vertex = VSInput;

using VertexShader = std::function<VSOut(VSInput*, void*)>;
using PixelShader = std::function<Color(PSInput*, void*)>;

struct PipelineState
{
	VertexShader vertexShader;
	PixelShader pixelShader;
	
	// for clipping
	//VSOut inVertexAttribute[10];
	//VSOut outVertexAttribute[10];
};