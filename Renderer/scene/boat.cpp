#include "boat.h"
#include "core/shader_functions.h"
#include "core/renderer.h"

VSOut BoatVS(VSInput* vsInput, void** cb)
{
	BoatCB* passCB = (BoatCB*)cb[0];
	VSOut vs_out;
	float4 view_pos = Mul(float4(vsInput->position, 1.0f), passCB->ViewMat);
	vs_out.sv_position = Mul(view_pos, passCB->ProjMat);
	vs_out.uv = vsInput->uv;
	return vs_out;
}

Color BoatPS(PSInput* psInput, void** cb)
{
	return Color(1.0, 1.0, 1.0, 1.0);
}

void Boat::InitScene(FrameBuffer* frameBuffer, Camera& camera)
{
	m_boatModel.LoadFromOBJ("assets/Fishing Boat/Boat.obj");
	camera.SetTarget(m_boatModel.GetCenter());
	camera.SetPosition(m_boatModel.GetCenter() + float3(m_boatModel.GetRadius()));

	m_pipelineState.VS = BoatVS;
	m_pipelineState.PS = BoatPS;
	RasterizerDesc& rs_desc = m_pipelineState.RasterizerState;
	rs_desc.CullMode = Cull_Mode_Back;
	rs_desc.FrontCounterClockWise = false;
	DepthStencilDesc& ds_desc = m_pipelineState.DepthStencilState;
	ds_desc.DepthEnable = true;
	ds_desc.DepthFunc = Comparison_Func_Less;

	m_frameBuffer = frameBuffer;

	m_depthBuffer = new DepthBuffer(m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight());
}

void Boat::Update(const Timer& timer, const IO& io, Camera& camera)
{
	m_passCB.ViewMat = camera.GetViewMatrix();
	m_passCB.ProjMat = camera.GetProjectionMatrix();
}

void Boat::Draw(GraphicsContext& context)
{
	context.ClearColor(m_frameBuffer, Color(0.2, 0.2, 0.2, 1.0));
	context.ClearDepth(m_depthBuffer, 1.0f);
	context.SetConstantBuffer(0, &m_passCB);
	context.SetRenderTarget(m_frameBuffer, m_depthBuffer); 
	context.SetPipelineState(&m_pipelineState);
	m_boatModel.Draw(context);
}

void Boat::Release()
{
	if (m_depthBuffer != nullptr)
		delete m_depthBuffer;
}