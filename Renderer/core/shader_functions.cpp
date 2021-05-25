#include "shader_functions.h"

namespace shader
{
	float smoothstep(float x0, float x1, float x)
	{
		float t = std::clamp((x - x0) / (x1 - x0), 0.0f, 1.0f);
		return t * t * (3.f - 2.f * t);
	}
}