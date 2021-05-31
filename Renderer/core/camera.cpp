#include "camera.h"

Camera::Camera(const vec3f& pos, const vec3f& target, float aspect)
	: m_position(pos), m_target(target), m_aspect(aspect)
{
	m_fov = ToRadians(90.f);
	m_up = vec3f(0.f, 1.f, 0.f);
	m_near = 0.01f;
	m_far = 10000.f;
}

void Camera::SetNear(float near)
{
	m_near = near;
}

void Camera::SetFar(float far)
{
	m_far = far;
}

void Camera::SetAspect(float aspect)
{
	m_aspect = aspect;
}

void Camera::SetPosition(const vec3f& pos)
{
	m_position = pos;
}

void Camera::SetTarget(const vec3f& target)
{
	m_target = target;
}

const vec3f& Camera::GetPosition() const
{
	return m_position;
}

const vec3f& Camera::GetTarget() const
{
	return m_target;
}