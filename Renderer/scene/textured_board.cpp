#include "textured_board.h"
#include "core/renderer.h"
#include "utils/timer.h"
#include "core/shader_functions.h"

VSOut TexturedBoardVS(VSInput* vsInput, void** cb)
{
	TexturedBoardCB* passCB = (TexturedBoardCB*)cb[0];
	VSOut vs_out;
	float4 view_pos = Mul(float4(vsInput->position, 1.0f), passCB->ViewMat);
	vs_out.sv_position = Mul(view_pos, passCB->ProjMat);
	vs_out.uv = vsInput->uv;
	return vs_out;
}

Color TexturedBoardPS(PSInput* psInput, void** cb, Texture** texs, SamplerState** samplers)
{
	return texs[0]->SampleLevel(*samplers[0], psInput->uv);
}

void TexturedBoard::InitScene(FrameBuffer* frameBuffer, Camera& camera)
{
	Vertex v;
	v.position = float3(-1.f, -1.f, 0.1f);
	v.normal = float3(0.f, 0.f, -1.f);
	v.uv = float2(0.f, 1.f);
	v.color = float4(1.f, 0.f, 0.f, 1.f);
	m_vertexBuffer.push_back(v);
	v.position = float3(1.f, -1.f, 0.1f);
	v.uv = float2(1.f, 1.f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-1.f, 1.f, 0.1f);
	v.uv = float2(0.f, 0.f);
	m_vertexBuffer.push_back(v);

	v.position = float3(1.f, 1.f, 0.1f);
	v.uv = float2(1.f, 0.f);
	v.color = float4(0.f, 0.f, 0.f, 0.1f);
	m_vertexBuffer.push_back(v);
	v.position = float3(1.f, -1.f, 0.1f);
	v.uv = float2(1.f, 1.f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-1.f, 1.f, 0.1f);
	v.uv = float2(0.f, 0.f);
	m_vertexBuffer.push_back(v);

	m_indexBuffer.push_back(0);
	m_indexBuffer.push_back(1);
	m_indexBuffer.push_back(2);
	m_indexBuffer.push_back(4);
	m_indexBuffer.push_back(3);
	m_indexBuffer.push_back(5);

	m_pipelineState.VS = TexturedBoardVS;
	m_pipelineState.PS = TexturedBoardPS;
	RasterizerDesc& rs_desc = m_pipelineState.RasterizerState;
	rs_desc.CullMode = Cull_Mode_Back;
	rs_desc.FrontCounterClockWise = true;

	m_frameBuffer = frameBuffer;

	camera.SetTarget(float3(0.0f, 0.0f, 0.0f));
	camera.SetPosition(float3(0.0f, 0.0f, -1.0f));

	m_texture.LoadFromTGA("assets/stone.tga");
	m_linearSampler.AddressU = Address_Mode_Warp;
	m_linearSampler.AddressV = Address_Mode_Warp;
	m_linearSampler.Filter = Filter_Min_Mag_Linear_Mip_Point;
}

void TexturedBoard::Update(const Timer& timer, const IO& io, Camera& camera)
{
	m_passCB.ViewMat = camera.GetViewMatrix();
	m_passCB.ProjMat = camera.GetProjectionMatrix();
}

void TexturedBoard::Draw(GraphicsContext& context)
{
	context.ClearColor(m_frameBuffer, Color(0.2, 0.2, 0.2, 1.0));
	context.SetConstantBuffer(0, &m_passCB);
	context.SetRenderTarget(m_frameBuffer);
	context.SetSRV(0, &m_texture);
	context.SetSampler(0, &m_linearSampler);
	context.SetPipelineState(&m_pipelineState);
	context.SetVertexBuffer(m_vertexBuffer.data());
	context.SetIndexBuffer(m_indexBuffer.data());
	context.DrawIndexed(6, 0, 0);
}

void TexturedBoard::Release()
{

}

void TexturedBoard::OnResize(int width, int height)
{

}