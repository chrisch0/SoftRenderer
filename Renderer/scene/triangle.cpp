#include "triangle.h"
#include "core/shader_functions.h"
#include "core/renderer.h"

VSOut TriangleVS(VSInput* vsInput, void** cb)
{
	TriangleCB* passCB = (TriangleCB*)cb[0];
	VSOut vs_out;
	float4 view_pos = Mul(float4(vsInput->position, 1.0f), passCB->ViewMat);
	vs_out.sv_position = Mul(view_pos, passCB->ProjMat);
	vs_out.uv = vsInput->uv;
	return vs_out;
}

Color TrianglePS(PSInput* psInput, void** cb, void** srvs, SamplerState** samplers)
{
	float p = (std::fmod(psInput->uv.x * 10.f, 1.0) > 0.5) ^ (std::fmod(psInput->uv.y * 10.f, 1.0) < 0.5);
	float2 col = psInput->uv * p;
	return Color(col.x, col.y, 0, 1.0);
}

void Triangle::InitScene(FrameBuffer* frameBuffer, Camera& camera)
{
	Vertex v;
	v.position = float3(-0.5f, 0.f, 0.f);
	v.color = float4(1.f, 1.f, 1.f, 1.f);
	v.uv = float2(1.f, 0.f);
	v.normal = float3(0.f, 0.f, 1.f);
	m_vertexBuffer.push_back(v);
	v.position = float3(0.f, 0.5f, 0.f);
	v.uv = float2(0.f, 0.f);
	m_vertexBuffer.push_back(v);
	v.position = float3(0.5f, 0.f, 0.f);
	v.uv = float2(0.f, 1.f);
	m_vertexBuffer.push_back(v);
	m_indexBuffer = { 0, 1, 2 };
	camera.SetPosition(float3(0.0, -0.0, -0.5));
	camera.SetTarget(float3(0.f, 0.16667, 0.f));

	m_pipelineState.VS = TriangleVS;
	m_pipelineState.PS = TrianglePS;
	RasterizerDesc& rs_desc = m_pipelineState.RasterizerState;
	rs_desc.CullMode = Cull_Mode_None;
	rs_desc.FrontCounterClockWise = false;

	m_frameBuffer = frameBuffer;
}

void Triangle::Update(const Timer& timer, const IO& io, Camera& camera)
{
	m_passCB.ViewMat = camera.GetViewMatrix();
	m_passCB.ProjMat = camera.GetProjectionMatrix();
}

void Triangle::Draw(GraphicsContext& context)
{
	context.ClearColor(m_frameBuffer, Color(0.2, 0.2, 0.2, 1.0));
	context.SetConstantBuffer(0, &m_passCB);
	context.SetRenderTarget(m_frameBuffer);
	context.SetPipelineState(&m_pipelineState);
	context.SetVertexBuffer(m_vertexBuffer.data());
	context.SetIndexBuffer(m_indexBuffer.data());
	context.DrawIndexed(3, 0, 0);
}

void Triangle::Release()
{

}

void Triangle::OnResize(int width, int height)
{

}