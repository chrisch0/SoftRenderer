#include "cube.h"
#include "core/shader_functions.h"
#include "core/renderer.h"

VSOut CubeVS(VSInput* vsInput, void** cb)
{
	CubeCB* passCB = (CubeCB*)cb[0];
	VSOut vs_out;
	float4 view_pos = Mul(float4(vsInput->position, 1.0f), passCB->ViewMat);
	vs_out.sv_position = Mul(view_pos, passCB->ProjMat);
	vs_out.uv = vsInput->uv;
	return vs_out;
}

Color CubePS(PSInput* psInput, void** cb, Texture** texs, SamplerState** samplers)
{
	float p = (std::fmod(psInput->uv.x * 10.f, 1.0) > 0.5) ^ (std::fmod(psInput->uv.y * 10.f, 1.0) < 0.5);
	return Color(p, p, p, 1.0);
}

void Cube::InitScene(FrameBuffer* frameBuffer, Camera& camera)
{
	m_cubeModel.LoadFromOBJ("assets/cube_no_tex.obj");

	Vertex v;
	v.position = float3(-0.5f, -0.5f, -0.5f);
	v.normal = float3(0.0f, 0.0f, -1.0f);
	v.uv = float2(0.0f, 1.0f);
	v.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, +0.5f, -0.5f);
	v.uv = float2(0.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, +0.5f, -0.5f);
	v.uv = float2(1.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, -0.5f, -0.5f);
	v.uv = float2(1.0f, 1.0f);
	m_vertexBuffer.push_back(v);

	v.position = float3(-0.5f, -0.5f, +0.5f);
	v.normal = float3(0.0f, 0.0f, 1.0f);
	v.uv = float2(1.0f, 1.0f);
	v.color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, -0.5f, +0.5f);
	v.uv = float2(0.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, +0.5f, +0.5f);
	v.uv = float2(0.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, +0.5f, +0.5f);
	v.uv = float2(1.0f, 0.0f);
	m_vertexBuffer.push_back(v);

	v.position = float3(-0.5f, +0.5f, -0.5f);
	v.normal = float3(0.0f, 1.0f, 0.0f);
	v.uv = float2(0.0f, 1.0f);
	v.color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, +0.5f, +0.5f);
	v.uv = float2(0.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, +0.5f, +0.5f);
	v.uv = float2(1.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, +0.5f, -0.5f);
	v.uv = float2(1.0f, 1.0f);
	m_vertexBuffer.push_back(v);

	v.position = float3(-0.5f, -0.5f, -0.5f);
	v.normal = float3(0.0f, -1.0f, 0.0f);
	v.uv = float2(1.0f, 1.0f);
	v.color = float4(1.0f, 1.0f, 0.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, -0.5f, -0.5f);
	v.uv = float2(0.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, -0.5f, +0.5f);
	v.uv = float2(0.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, -0.5f, +0.5f);
	v.uv = float2(1.0f, 0.0f);
	m_vertexBuffer.push_back(v);

	v.position = float3(-0.5f, -0.5f, +0.5f);
	v.normal = float3(-1.0f, 0.0f, 0.0f);
	v.uv = float2(0.0f, 1.0f);
	v.color = float4(0.0f, 1.0f, 1.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, +0.5f, +0.5f);
	v.uv = float2(0.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, +0.5f, -0.5f);
	v.uv = float2(1.0f, 0.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(-0.5f, -0.5f, -0.5f);
	v.uv = float2(1.0f, 1.0f);
	m_vertexBuffer.push_back(v);


	v.position = float3(+0.5f, -0.5f, -0.5f);
	v.normal = float3(1.0f, 0.0f, 0.0f);
	v.uv = float2(0.0f, 1.0f);
	v.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, +0.5f, -0.5f);
	v.uv = float2(0.0f, 0.0);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, +0.5f, +0.5f);
	v.uv = float2(1.0f, 0.0);
	m_vertexBuffer.push_back(v);
	v.position = float3(+0.5f, -0.5f, +0.5f);
	v.uv = float2(1.0f, 1.0);
	m_vertexBuffer.push_back(v);

	m_indexBuffer = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	//camera.SetPosition(float3(0.f, 0.0f, -1.5f));
	camera.SetTarget(m_cubeModel.GetCenter());
	camera.SetPosition(m_cubeModel.GetCenter() + float3(m_cubeModel.GetRadius()));

	m_pipelineState.VS = CubeVS;
	m_pipelineState.PS = CubePS;
	RasterizerDesc& rs_desc = m_pipelineState.RasterizerState;
	rs_desc.CullMode = Cull_Mode_None;
	rs_desc.FrontCounterClockWise = false;
	DepthStencilDesc& ds_desc = m_pipelineState.DepthStencilState;
	ds_desc.DepthEnable = true;
	ds_desc.DepthFunc = Comparison_Func_Less;

	m_frameBuffer = frameBuffer;

	m_depthBuffer = new DepthBuffer(m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());
}

void Cube::Update(const Timer& timer, const IO& io, Camera& camera)
{
	m_passCB.ViewMat = camera.GetViewMatrix();
	m_passCB.ProjMat = camera.GetProjectionMatrix();
}

void Cube::Draw(GraphicsContext& context)
{
	context.ClearColor(m_frameBuffer, Color(0.2, 0.2, 0.2, 1.0));
	context.ClearDepth(m_depthBuffer, 1.0f);
	context.SetConstantBuffer(0, &m_passCB);
	context.SetRenderTarget(m_frameBuffer, m_depthBuffer);
	context.SetPipelineState(&m_pipelineState);
	/*context.SetVertexBuffer(m_vertexBuffer.data());
	context.SetIndexBuffer(m_indexBuffer.data());
	context.DrawIndexed(36, 0, 0);*/
	m_cubeModel.Draw(context);
}

void Cube::Release()
{
	if (m_depthBuffer != nullptr)
		delete m_depthBuffer;	
}

void Cube::OnResize(int width, int height)
{
	delete m_depthBuffer;
	m_depthBuffer = new DepthBuffer(width, height);
}