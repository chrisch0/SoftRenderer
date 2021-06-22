#pragma once
#include "scene.h"
#include "core/graphics.h"
#include "core/camera.h"
#include "core/model.h"

struct BoatCB
{
	float4x4 ViewMat;
	float4x4 ProjMat;
};

class Boat : public Scene
{
public:
	virtual void InitScene(FrameBuffer* frameBuffer, Camera& camera) override;
	virtual void Update(const Timer& timer, const IO& io, Camera& camera) override;
	virtual void Draw(GraphicsContext& context) override;
	virtual void Release() override;

private:
	BoatCB m_passCB;
	FrameBuffer* m_frameBuffer;
	DepthBuffer* m_depthBuffer;
	PipelineState m_pipelineState;
	Model m_boatModel;
};