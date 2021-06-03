#pragma once
#include "math/math.h"

class Camera
{
public:
	Camera(const float3& pos, const float3& target, float aspect);
	~Camera() {}

	void SetFOV(float fovy);
	void SetNear(float near);
	void SetFar(float far);
	void SetAspect(float aspect);
	void SetPosition(const float3& pos);
	void SetTarget(const float3& target);
	const float3& GetPosition() const;
	const float3& GetTarget() const;
	void Update(const float4& deltaCursor);
	void UpdateProjectionMatrix();

private:
	float3 m_position;
	float3 m_target;
	float m_near;
	float m_far;
	float m_fov;
	float m_aspect;
	float3 m_up;
};