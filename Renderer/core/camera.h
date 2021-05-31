#pragma once
#include "math/math.h"

class Camera
{
public:
	Camera(const vec3f& pos, const vec3f& target, float aspect);
	~Camera() {}

	void SetNear(float near);
	void SetFar(float far);
	void SetAspect(float aspect);
	void SetPosition(const vec3f& pos);
	void SetTarget(const vec3f& target);
	const vec3f& GetPosition() const;
	const vec3f& GetTarget() const;
	void Update();

private:
	vec3f m_position;
	vec3f m_target;
	float m_near;
	float m_far;
	float m_fov;
	float m_aspect;
	vec3f m_up;
};