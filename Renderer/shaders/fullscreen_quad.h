#pragma once
#include "core/graphics.h"
#include "core/shader_functions.h"

VSOut FullScreenQuadVS(VSInput* vsInput, void* cb)
{
	VSOut vsOut;
	vsOut.sv_position = vec4f(vsInput->position, 1.0f);
	vsOut.normal = vsInput->normal;
	vsOut.uv = vsInput->uv;
	vsOut.positionWS = vsInput->position;
	vsOut.color = vsInput->color;
	return vsOut;
}

Color FullScreenQuadPS(PSInput* psInput, void* cb)
{
	ConstantBuffer* passCb = (ConstantBuffer*)cb;
	vec2f uv = psInput->uv;
	vec4f mouse = passCb->mouse / vec4f(passCb->resolution.x, passCb->resolution.y, passCb->resolution.x, passCb->resolution.y);

	vec3f col = 0.5f + 0.5f * cos(passCb->time.x + vec3f(uv.x, uv.y, uv.x) + vec3f(0, 2, 4));
	//vec3f col = vec3f(uv, 0.f);
	float ratio_aspect = passCb->resolution.x / passCb->resolution.y;
	uv.x *= ratio_aspect;
	mouse.x *= ratio_aspect;
	mouse.z *= ratio_aspect;
	col += shader::smoothstep(0.05f, 0.01f, (uv - vec2f(mouse.x, mouse.y)).Length());
	return Color(col, 1.0f);
}