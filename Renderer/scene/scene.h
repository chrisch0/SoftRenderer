#pragma once
#include "core/pixel_buffer.h"

class Timer;
class GraphicsContext;
struct IO;
class Camera;

class Scene
{
public:
	virtual void InitScene(FrameBuffer* frameBuffer, Camera& camera) = 0;
	virtual void Update(const Timer& timer, const IO& io, Camera& camera) = 0;
	virtual void Draw(GraphicsContext& context) = 0;
	virtual void Release() = 0;
	virtual void OnResize(int width, int height) = 0;
};