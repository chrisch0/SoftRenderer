#pragma once
#include "scene.h"
#include "core/graphics.h"

struct FullScreenQuadCB
{
	float4 Resolution;
	float2 Time;
	float4 Mouse;
};

class FullScreenQuad : public Scene
{
public:
	virtual void InitScene(FrameBuffer* fameBuffer, Camera& camera) override;
	virtual void Update(const Timer& timer, const IO& io, Camera& camera) override;
	virtual void Draw(GraphicsContext& context) override;
	virtual void Release() override;
	virtual void OnResize(int width, int height) override;

private:
	std::vector<Vertex> m_vertexBuffer;
	std::vector<uint32_t> m_indexBuffer;
	FullScreenQuadCB m_passCB;
	FrameBuffer* m_frameBuffer;
	PipelineState m_pipelineState;
};