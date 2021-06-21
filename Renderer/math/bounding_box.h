#pragma once
#include "vec.h"
#include <limits>

struct BoundingBox3D
{
	BoundingBox3D() : BoxMin(float3(std::numeric_limits<float>::max())), BoxMax(float3(std::numeric_limits<float>::lowest())) {}
	void Min(const float3& p) { BoxMin = ::Min(BoxMin, p); }
	void Max(const float3& p) { BoxMax = ::Max(BoxMax, p); }
	float3 BoxMin;
	float3 BoxMax;
};