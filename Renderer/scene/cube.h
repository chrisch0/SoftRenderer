#pragma once
#include "scene.h"
#include "core/graphics.h"
#include "core/camera.h"

struct CubeCB
{
	float4x4 ViewMat;
	float4x4 ProjMat;
};

class Cube : public Scene
{
public:
	virtual void InitScene(FrameBuffer* frameBuffer, Camera& camera) override;
	virtual void Update(const Timer& timer, const IO& io, Camera& camera) override;
	virtual void Draw(GraphicsContext& context) override;
	virtual void Release() override;

private:
	std::vector<Vertex> m_vertexBuffer;
	std::vector<uint32_t> m_indexBuffer;
	CubeCB m_passCB;
	FrameBuffer* m_frameBuffer;
	DepthBuffer* m_depthBuffer;
	PipelineState m_pipelineState;
};