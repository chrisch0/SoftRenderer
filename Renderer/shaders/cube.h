#pragma once
#include "core/graphics.h"
#include "core/shader_functions.h"

VSOut CubeVS(VSInput* vsInput, void* cb)
{
	ConstantBuffer* passCB = (ConstantBuffer*)cb;
	VSOut vs_out;
	float4 view_pos = Mul(float4(vsInput->position, 1.0), passCB->viewMat);
	vs_out.sv_position = Mul(view_pos, passCB->projMat);
	vs_out.color = vsInput->color;
	vs_out.normal = vsInput->normal;
	vs_out.uv = vsInput->uv;
	vs_out.positionWS = vsInput->position;
	return vs_out;
}

Color CubePS(PSInput* psInput, void* cb)
{
	return psInput->color;
}