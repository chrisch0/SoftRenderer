#include "core/renderer.h"
#include "shaders/fullscreen_quad.h"

int main(int argc, const char* argv[])
{
	Renderer renderer(640, 360);
	renderer.Initialize(FullScreenQuadVS, FullScreenQuadPS);
	renderer.InitScene();
	renderer.MainLoop();
	return 0;
}