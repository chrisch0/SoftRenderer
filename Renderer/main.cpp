#include "core/renderer.h"
#include "shaders/fullscreen_quad.h"
#include "shaders/cube.h"

int main(int argc, const char* argv[])
{
	Renderer renderer(640, 360);
	renderer.Initialize(CubeVS, CubePS);
	renderer.InitScene();
	renderer.MainLoop();
	return 0;
}