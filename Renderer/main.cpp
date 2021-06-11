#include "core/renderer.h"
#include "scene/fullscreen_quad.h"
#include "scene/triangle.h"
#include "scene/cube.h"

int main(int argc, const char* argv[])
{
	Renderer renderer(640, 360);
	FullScreenQuad full_scene_quad;
	Triangle triangle;
	Cube cube;
	renderer.Initialize(&cube);
	renderer.InitScene();
	renderer.MainLoop();
	return 0;
}