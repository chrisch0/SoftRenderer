#pragma once
#include "vec.h"
#include <limits>

struct BoundingBox3D
{
	BoundingBox3D() : BoxMin(float3(FLT_MAX)), BoxMax(float3(-FLT_MAX)) {}
	void Min(const float3& p) { BoxMin = ::Min(BoxMin, p); }
	void Max(const float3& p) { BoxMax = ::Max(BoxMax, p); }
	float3 BoxMin;
	float3 BoxMax;
};