#pragma once
#include "core/graphics.h"
#include "core/shader_functions.h"

VSOut FullScreenQuadVS(VSInput* vsInput, void* cb)
{
	VSOut vsOut;
	vsOut.sv_position = float4(vsInput->position, 1.0f);
	vsOut.normal = vsInput->normal;
	vsOut.uv = vsInput->uv;
	vsOut.positionWS = vsInput->position;
	vsOut.color = vsInput->color;
	return vsOut;
}

Color FullScreenQuadPS(PSInput* psInput, void* cb)
{
	ConstantBuffer* passCb = (ConstantBuffer*)cb;
	float2 uv = psInput->uv;
	float4 mouse = passCb->Mouse / float4(passCb->Resolution.x, passCb->Resolution.y, passCb->Resolution.x, passCb->Resolution.y);

	float3 col = 0.5f + 0.5f * Cos(passCb->Time.x + float3(uv.x, uv.y, uv.x) + float3(0, 2, 4));
	//float3 col = float3(uv, 0.f);
	float ratio_aspect = passCb->Resolution.x / passCb->Resolution.y;
	uv.x *= ratio_aspect;
	mouse.x *= ratio_aspect;
	mouse.z *= ratio_aspect;
	col += shader::smoothstep(0.05f, 0.01f, (uv - float2(mouse.x, mouse.y)).Length());
	return Color(col, 1.0f);
}