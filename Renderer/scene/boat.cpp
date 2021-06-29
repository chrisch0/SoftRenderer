#include "boat.h"
#include "core/shader_functions.h"
#include "core/renderer.h"
#include <cmath>

VSOut ShadowVS(VSInput* vsInput, void** cb)
{
	BoatPassCB* passCB = (BoatPassCB*)cb[0];
	VSOut vs_out;
	vs_out.sv_position = Mul(float4(vsInput->position, 1.0f), passCB->DirectLightMVP);
	return vs_out;
}

VSOut BoatVS(VSInput* vsInput, void** cb)
{
	BoatPassCB* passCB = (BoatPassCB*)cb[0];
	VSOut vs_out;
	float4 view_pos = Mul(float4(vsInput->position, 1.0f), passCB->ViewMat);
	vs_out.sv_position = Mul(view_pos, passCB->ProjMat);
	vs_out.uv = vsInput->uv;
	vs_out.normal = vsInput->normal;
	vs_out.tangent = vsInput->tangent;
	vs_out.bitangent = vsInput->bitangent;
	vs_out.positionWS = vsInput->position;
	vs_out.positionLS = Mul(float4(vsInput->position, 1.0f), passCB->DirectLightMVP);
	return vs_out;
}

int offsets[3] = {
	-1, 0, 1
};

float PCF(const DepthBuffer* shadowMap, const float4& posLS)
{
	float3 shadow_coord = float3(posLS / posLS.w);
	float depth = shadow_coord.z;
	float u = 0.5f * shadow_coord.x + 0.5f;
	float v = -0.5f * shadow_coord.y + 0.5f;
	int x = (int)(u * shadowMap->GetWidth() - 0.5f);
	int y = (int)(v * shadowMap->GetHeight() - 0.5f);

	float r = 0.0f;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			float pcfDepth = shadowMap->GetValue(x + offsets[i], y + offsets[j]);
			r += (depth - 0.001f < pcfDepth ? 1.0f : 0.0f);
		}
	}

	return r / 9.0f;
}

Color BoatPS(PSInput* psInput, void** cb, void** srvs, SamplerState** samplers)
{
	const SamplerState& linear_sampler = *samplers[0];
	const Texture& ambient_tex = *(Texture*)srvs[0];
	const Texture& normal_tex = *(Texture*)srvs[1];
	const Texture& specular_tex = *(Texture*)srvs[2];
	const DepthBuffer& shadow_map = *(DepthBuffer*)srvs[3];

	float shadow = PCF(&shadow_map, psInput->positionLS);

	float4 normal_sampled = normal_tex.SampleLevel(linear_sampler, psInput->uv);
	float3 normal_ts = float3(normal_sampled) * 2.0f - float3(1.0f);
	float3x3 TBN(psInput->tangent, psInput->bitangent, psInput->normal);
	float3 normal_ws = Mul(normal_ts, TBN);

	BoatPassCB* passCB = (BoatPassCB*)cb[0];
	BoatMat * matCB = (BoatMat*)cb[1];
	Color ambient_sampled = ambient_tex.SampleLevel(linear_sampler, psInput->uv);
	float3 ambient = Mul(float3(ambient_sampled), passCB->LightColor) * 0.05f;

	float diff = Dot(psInput->normal, passCB->LightDir);
	diff = diff < 0.0f ? 0.0f : diff;
	float3 diffuse = Mul(passCB->LightColor, float3(ambient_sampled)) * passCB->LightIntensity * diff;

	float3 view_dir = Normalize(passCB->ViewPos - psInput->positionWS);
	float3 half_vec = Normalize(passCB->LightDir + view_dir);
	float spec = Dot(normal_ws, half_vec);
	spec = spec < 0.0f ? 0.0f : spec;
	spec = std::pow(spec, matCB->IndexOfRefraction);
	float3 specular = float3(specular_tex.SampleLevel(linear_sampler, psInput->uv)) * spec;

	//return normal_sampled;
	//return Color(shadow, shadow, shadow, 1.0);
	return Color((specular + diffuse) * shadow, 1.0);
	//return texs[0]->SampleLevel(*samplers[0], psInput->uv);
}

void Boat::InitScene(FrameBuffer* frameBuffer, Camera& camera)
{
	m_boatModel.LoadFromOBJ("assets/Fishing Boat/Boat.obj");
	//camera.SetWidth(1400.f);
	//camera.SetHeight(1400.0f);
	//camera.SetCameraType(false);
	//camera.SetPosition(m_boatModel.GetCenter() + float3(-m_boatModel.GetRadius(), m_boatModel.GetRadius(), -m_boatModel.GetRadius()));
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

	// set up shadow 
	m_directionalLight.SetWidth(1400.0f);
	m_directionalLight.SetHeight(1400.0f);
	m_directionalLight.SetCameraType(false);
	m_directionalLight.SetTarget(m_boatModel.GetCenter());
	m_directionalLight.SetPosition(m_boatModel.GetCenter() + float3(-m_boatModel.GetRadius(), m_boatModel.GetRadius(), -m_boatModel.GetRadius()));
	m_directionalLight.UpdateViewMatrix();

	m_shadowTestState.VS = ShadowVS;
	m_shadowTestState.PS = nullptr;
	RasterizerDesc& shadow_rs = m_shadowTestState.RasterizerState;
	shadow_rs.CullMode = Cull_Mode_Back;
	shadow_rs.FrontCounterClockWise = false;
	DepthStencilDesc& shadow_ds = m_shadowTestState.DepthStencilState;
	shadow_ds.DepthEnable = true;
	shadow_ds.DepthFunc = Comparison_Func_Less;

	m_shadowMap = new DepthBuffer(512, 512);

	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}

void Boat::Update(const Timer& timer, const IO& io, Camera& camera)
{
	//float theta = 2.0f * PI * timer.TotalTime() * 0.5f;
	//camera.SetPosition(float3(std::cos(theta), std::sin(theta), 0.0) * m_boatModel.GetRadius());
	m_passCB.ViewMat = m_directionalLight.GetViewMatrix();
	m_passCB.ProjMat = m_directionalLight.GetProjectionMatrix();
	m_passCB.LightColor = float3(1.0f);
	m_passCB.LightDir = Normalize(m_directionalLight.GetPosition() - m_directionalLight.GetTarget());
	m_passCB.LightIntensity = 1.0f;
	m_passCB.DirectLightMVP = m_directionalLight.GetViewMatrix() * m_directionalLight.GetProjectionMatrix();
}

void Boat::Draw(GraphicsContext& context)
{
	// shadow pass
	m_viewport.Height = 512;
	m_viewport.Width = 512;
	context.SetViewport(&m_viewport);
	context.ClearDepth(m_shadowMap, 1.0f);
	context.SetConstantBuffer(0, &m_passCB);
	context.SetRenderTarget(nullptr, m_shadowMap);
	context.SetPipelineState(&m_pipelineState);
	m_boatModel.Draw(context);

	m_viewport.Width = m_frameBuffer->GetWidth();
	m_viewport.Height = m_frameBuffer->GetHeight();
	context.SetViewport(&m_viewport);
	context.ClearColor(m_frameBuffer, Color(0.2, 0.2, 0.2, 1.0));
	context.ClearDepth(m_depthBuffer, 1.0f);
	context.SetRenderTarget(m_frameBuffer, m_depthBuffer); 
	context.SetPipelineState(&m_pipelineState);
	context.SetSRV(3, m_shadowMap);
	auto set_mat_cxt = [&](Material* pMat) -> void
	{
		context.SetSRV(0, pMat->pAmbientMap);
		context.SetSRV(1, pMat->pBumpMap1);
		context.SetSRV(2, pMat->pSpecularMap);
		m_matCB.IndexOfRefraction = pMat->IndexOfRefraction;
		context.SetConstantBuffer(1, &m_matCB);
		context.SetSampler(0, &m_linearSampler);
	};
	m_boatModel.Draw(context, set_mat_cxt);
}

void Boat::Release()
{
	if (m_depthBuffer != nullptr)
		delete m_depthBuffer;

	if (m_shadowMap != nullptr)
		delete m_shadowMap;
}

void Boat::OnResize(int width, int height)
{
	delete m_depthBuffer;
	m_depthBuffer = new DepthBuffer(width, height);
}