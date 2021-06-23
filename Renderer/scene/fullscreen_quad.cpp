#include "fullscreen_quad.h"
#include "utils/timer.h"
#include "core/renderer.h"
#include "core/shader_functions.h"

VSOut FullScreenQuadVS(VSInput* vsInput, void** cb)
{
	VSOut vsOut;
	vsOut.sv_position = float4(vsInput->position, 1.0f);
	vsOut.normal = vsInput->normal;
	vsOut.uv = vsInput->uv;
	vsOut.positionWS = vsInput->position;
	vsOut.color = vsInput->color;
	return vsOut;
}

Color FullScreenQuadPS(PSInput* psInput, void** cb)
{
	FullScreenQuadCB* passCb = (FullScreenQuadCB*)cb[0];
	float2 uv = psInput->uv;
	float4 mouse = passCb->Mouse / float4(passCb->Resolution.x, passCb->Resolution.y, passCb->Resolution.x, passCb->Resolution.y);

	float3 col = 0.5f + 0.5f * Cos(passCb->Time.x + float3(uv.x, uv.y, uv.x) + float3(0, 2, 4));
	//float3 col = float3(uv, 0.f);
	float ratio_aspect = passCb->Resolution.x / passCb->Resolution.y;
	uv.x *= ratio_aspect;
	mouse.x *= ratio_aspect;
	mouse.z *= ratio_aspect;
	col += float3(shader::smoothstep(0.05f, 0.01f, (uv - float2(mouse.x, mouse.y)).Length()));
	return Color(col, 1.0f);
}

void FullScreenQuad::InitScene(FrameBuffer* frameBuffer, Camera& cameras)
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

	m_pipelineState.VS = FullScreenQuadVS;
	m_pipelineState.PS = FullScreenQuadPS;
	RasterizerDesc& rs_desc = m_pipelineState.RasterizerState;
	rs_desc.CullMode = Cull_Mode_Back;
	rs_desc.FrontCounterClockWise = true;

	m_frameBuffer = frameBuffer;
}

void FullScreenQuad::Update(const Timer& timer, const IO& io, Camera& camera)
{
	m_passCB.Resolution = float4(m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight(), 1.f / m_frameBuffer->GetWidth(), 1.f / m_frameBuffer->GetHeight());
	m_passCB.Mouse = float4(io.CurMousePos.x, io.CurMousePos.y, io.ClickMousePos.x, io.ClickMousePos.y);
	m_passCB.Time = float2(timer.TotalTime(), timer.DeltaTime());
}

void FullScreenQuad::Draw(GraphicsContext& context)
{
	context.ClearColor(m_frameBuffer, Color(0.0f, 0.0f, 0.0f, 0.0f));
	context.SetConstantBuffer(0, &m_passCB);
	context.SetRenderTarget(m_frameBuffer);
	context.SetPipelineState(&m_pipelineState);
	context.SetVertexBuffer(m_vertexBuffer.data());
	context.SetIndexBuffer(m_indexBuffer.data());
	context.DrawIndexed(6, 0, 0);
}

void FullScreenQuad::Release()
{

}