#include "graphics.h"
#include "math/math.h"

void PSInput::LerpAssgin(const PSInput& v0, const PSInput& v1, float t)
{
	sv_position = Lerp(v0.sv_position, v1.sv_position, t);
	normal = Lerp(v0.normal, v1.normal, t);
	positionWS = Lerp(v0.positionWS, v1.positionWS, t);
	uv = Lerp(v0.uv, v1.uv, t);
	color = Lerp(v0.color, v1.color, t);
}

bool InsideClippingPlane(eHomoClippingPlane plane, const float4& coord)
{
	switch (plane)
	{
	case Positive_W:
		return coord.w >= 1e-5f;
	case Positive_X:
		return coord.x <= coord.w;
	case Negative_X:
		return coord.x >= -coord.w;
	case Positive_Y:
		return coord.y <= coord.w;
	case Negative_Y:
		return coord.y >= -coord.w;
	case Positive_Z:
		return coord.z <= coord.w;
	case Negative_Z:
		return coord.z >= 1e-5f;
	default:
		return false;
	}
}

float LineSegmentIntersectClippingPlane(eHomoClippingPlane plane, const float4& p0, const float4& p1)
{
	switch (plane)
	{
	case Positive_W:
		return (p0.w - 1e-5f) / (p0.w - p1.w);
	case Positive_X:
		return (p0.w - p0.x) / ((p0.w - p0.x) - (p1.w - p1.x));
	case Negative_X:
		return (p0.w + p0.x) / ((p0.w + p0.x) - (p1.w + p1.x));
	case Positive_Y:
		return (p0.w - p0.y) / ((p0.w - p0.y) - (p1.w - p1.y));
	case Negative_Y:
		return (p0.w + p0.y) / ((p0.w + p0.y) - (p1.w + p1.y));
	case Positive_Z:
		return (p0.w - p0.z) / ((p0.w - p0.z) - (p1.w - p1.z));
	case Negative_Z:
		return (p0.z - 1e-5f) / (p0.z - p1.z);
	default:
		return 0.0f;
	}
}