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
	void LerpAssgin(const PSInput& v0, const PSInput& v1, float t);
};

struct ConstantBuffer
{
	float4 Resolution;
	float2 Time;
	float4 Mouse;
	float4x4 ViewMat;
	float4x4 ProjMat;
};

using VSOut = PSInput;
using Vertex = VSInput;

using VertexShader = std::function<VSOut(VSInput*, void*)>;
using PixelShader = std::function<Color(PSInput*, void*)>;

struct Viewport
{
	float TopLeftX;
	float TopLeftY;
	float Width;
	float Height;
	float MinDepth;
	float MaxDepth;
};

enum eFillMode
{
	Fill_Mode_Wireframe,
	Fill_Mode_Solid
};

enum eCullMode
{
	Cull_Mode_None,
	Cull_Mode_Front,
	Cull_Mode_Back
};

struct RasterizerDesc
{
	eFillMode FillMode;
	eCullMode CullMode;
	bool FrontCounterClockWise;
};

struct PipelineState
{
	VertexShader VS;
	PixelShader PS;
	RasterizerDesc RasterizerState;
	
	// for clipping
	//VSOut inVertexAttribute[10];
	//VSOut outVertexAttribute[10];
};


enum eHomoClippingPlane
{
	Positive_W,
	Positive_X,
	Negative_X,
	Positive_Y,
	Negative_Y,
	Positive_Z,
	Negative_Z,
	Clipping_Plane_Count
};

bool InsideClippingPlane(eHomoClippingPlane plane, const float4& coord);

float LineSegmentIntersectClippingPlane(eHomoClippingPlane plane, const float4& p0, const float4& p1);