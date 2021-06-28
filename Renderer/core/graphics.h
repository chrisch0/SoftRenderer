#pragma once
#include <functional>
#include <array>
#include "math/math.h"
#include "pixel_buffer.h"
#include "texture.h"
#include "sampler.h"

#define MAX_RENDER_TARGET 8

struct VSInput
{
	float3 position;
	float2 uv;
	float3 normal;
	float3 tangent;
	float3 bitangent;
	Color color;
};

struct PSInput
{
	float4 sv_position;
	float3 normal;
	float3 positionWS;
	float2 uv;
	float4 color;
	float3 tangent;
	float3 bitangent;
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

using VertexShader = std::function<VSOut(VSInput*, void**)>;
using PixelShader = std::function<Color(PSInput*, void**, Texture**, SamplerState**)>;

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

enum eDepthFunc
{
	Comparison_Func_Never,
	Comparison_Func_Less,
	Comparison_Func_Equal,
	Comparison_Func_Less_Equal,
	Comparison_Func_Greater,
	Comparison_Func_Not_Equal,
	Comparison_Func_Greater_Equal,
	Comparison_Func_Always
};

struct RasterizerDesc
{
	eFillMode FillMode = Fill_Mode_Solid;
	eCullMode CullMode = Cull_Mode_None;
	bool FrontCounterClockWise = true;
};

// TODO: actually only has depth test now
struct DepthStencilDesc
{
	bool DepthEnable = false;
	eDepthFunc DepthFunc = Comparison_Func_Always;
	//bool StencilEnable;
	//uint8_t StencilReadMask;
	//uint8_t StencilWriteMask;

};

struct PipelineState
{
	VertexShader VS;
	PixelShader PS;
	RasterizerDesc RasterizerState;
	DepthStencilDesc DepthStencilState;
};

class GraphicsContext
{
public:
	void SetRenderTarget(FrameBuffer* frameBuffer, DepthBuffer* depthBuffer = nullptr, StencilBuffer* stencilBuffer = nullptr)
	{
		m_numRTs = 1;
		m_frameBuffer = frameBuffer;
		m_depthBuffer = depthBuffer;
	}
	void SetRenderTargets(FrameBuffer* frameBuffer, uint8_t numRTs, ColorBuffer* colorBuffers[], DepthBuffer* depthBuffer = nullptr, StencilBuffer* stencilBuffer = nullptr)
	{
		m_frameBuffer = frameBuffer;
		m_numRTs = numRTs + 1;
		for (int i = 0; i < numRTs; ++i)
		{
			m_multiRenderTargets[i] = colorBuffers[i];
		}
		m_depthBuffer = depthBuffer;
	}
	void SetPipelineState(PipelineState* pso)
	{
		m_pipelineState = pso;
	}
	void SetVertexBuffer(Vertex* vertexBuffer)
	{
		m_vertexBuffer = vertexBuffer;
	}
	void SetIndexBuffer(uint32_t* indexBuffer)
	{
		m_indexBuffer = indexBuffer;
	}
	void SetConstantBuffer(size_t slot, void* cb)
	{
		m_constantBuffer[slot] = cb;
	}
	void SetSRV(size_t slot, Texture* tex)
	{
		m_textureSlots[slot] = tex;
	}
	void SetSampler(size_t slot, SamplerState* sampler)
	{
		m_samplerSlots[slot] = sampler;
	}
	void SetViewport(Viewport* viewport)
	{
		m_viewport = viewport;
	}
	void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation = 0, uint32_t baseVertexLocation = 0);

	void ClearDepth(DepthBuffer* depthBuffer, float value);
	void ClearColor(FrameBuffer* frameBuffer, const Color& value);
	void ClearColor(ColorBuffer* colorBuffer, const Color& value);

private:
	FrameBuffer* m_frameBuffer;
	ColorBuffer* m_multiRenderTargets[MAX_RENDER_TARGET];
	DepthBuffer* m_depthBuffer;
	uint8_t m_numRTs = 0;
	Vertex* m_vertexBuffer;
	uint32_t* m_indexBuffer;
	PipelineState* m_pipelineState;
	Viewport* m_viewport;
	void* m_constantBuffer[10];
	Texture* m_textureSlots[10];
	SamplerState* m_samplerSlots[10];
};