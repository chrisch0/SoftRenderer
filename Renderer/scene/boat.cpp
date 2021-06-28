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
	vs_out.normal = vsInput->normal;
	vs_out.tangent = vsInput->tangent;
	vs_out.bitangent = vsInput->bitangent;
	return vs_out;
}

Color BoatPS(PSInput* psInput, void** cb, Texture** texs, SamplerState** samplers)
{
	const SamplerState& linear_sampler = *samplers[0];
	const Texture& ambient_tex = *texs[0];
	const Texture& normal_tex = *texs[1];
	float4 normal_sampled = normal_tex.SampleLevel(linear_sampler, psInput->uv);
	float3 normal_t = float3(normal_sampled) * 2.0f - float3(1.0f);

	return Color(psInput->normal, 1.0);
	//return texs[0]->SampleLevel(*samplers[0], psInput->uv);
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

	auto set_mat_cxt = [&](Material* pMat) -> void
	{
		context.SetSRV(0, pMat->pAmbientMap);
		context.SetSampler(0, &m_linearSampler);
	};
	m_boatModel.Draw(context, set_mat_cxt);
}

void Boat::Release()
{
	if (m_depthBuffer != nullptr)
		delete m_depthBuffer;
}

void Boat::OnResize(int width, int height)
{
	delete m_depthBuffer;
	m_depthBuffer = new DepthBuffer(width, height);
}