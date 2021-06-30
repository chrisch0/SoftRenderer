#pragma once
#include "scene.h"
#include "core/graphics.h"
#include "core/camera.h"
#include "core/model.h"

struct BoatPassCB
{
	float4x4 ViewMat;
	float4x4 ProjMat;
	float4x4 DirectLightMVP;
	float4x4 DirectLightProj;
	float3 ViewPos;
	float3 LightDir;
	float3 LightColor;
	float LightIntensity;
};

struct BoatMat
{
	float IndexOfRefraction;
};

class Boat : public Scene
{
public:
	virtual void InitScene(FrameBuffer* frameBuffer, Camera& camera) override;
	virtual void Update(const Timer& timer, const IO& io, Camera& camera) override;
	virtual void Draw(GraphicsContext& context) override;
	virtual void Release() override;
	virtual void OnResize(int width, int height) override;
private:
	BoatPassCB m_passCB;
	BoatMat m_matCB;
	Camera m_directionalLight;
	FrameBuffer* m_frameBuffer;
	DepthBuffer* m_depthBuffer = nullptr;
	DepthBuffer* m_shadowMap = nullptr;
	PipelineState m_pipelineState;
	PipelineState m_shadowTestState;
	Model m_boatModel;
	Model m_quad;
	SamplerState m_linearSampler;
	Viewport m_viewport;
};