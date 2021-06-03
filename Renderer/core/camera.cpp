#include "camera.h"

Camera::Camera(const float3& pos, const float3& target, float aspect)
	: m_position(pos), m_target(target), m_aspect(aspect)
{
	m_fov = ToRadians(90.f);
	m_up = float3(0.f, 1.f, 0.f);
	m_near = 0.01f;
	m_far = 10000.f;
}

void Camera::SetFOV(float fovy)
{
	m_fov = fovy;
	UpdateProjectionMatrix();
}

void Camera::SetNear(float near)
{
	m_near = near;
	UpdateProjectionMatrix();
}

void Camera::SetFar(float far)
{
	m_far = far;
	UpdateProjectionMatrix();
}

void Camera::SetAspect(float aspect)
{
	m_aspect = aspect;
	UpdateProjectionMatrix();
}

void Camera::SetPosition(const float3& pos)
{
	m_position = pos;
}

void Camera::SetTarget(const float3& target)
{
	m_target = target;
}

const float3& Camera::GetPosition() const
{
	return m_position;
}

const float3& Camera::GetTarget() const
{
	return m_target;
}

void Camera::Update(const float4& deltaCursor)
{
	float3 look_at = m_target - m_position;
	float3 forward = Normalize(look_at);
	float3 right = Cross(float3(0.f, 1.f, 0.f), forward);
	m_up = Cross(forward, right);

	// update pan
	float distance = look_at.Length();
	float factor = distance * (float)std::tan(m_fov * 0.5) * 2.f;
	float3 delta_x = right * deltaCursor.x * factor;
	float3 delta_y = m_up * deltaCursor.y * factor;
	m_target = m_target + delta_x + delta_y;

	// update orbit
	float theta = (float)std::atan2(-forward.x, -forward.z);
	float phi = (float)std::acos(-forward.y / distance);
	factor = PI * 2.f;

	theta -= deltaCursor.x * factor;
	phi -= deltaCursor.y * factor;
	phi = std::clamp<float>(phi, -std::numeric_limits<float>::epsilon(), PI + std::numeric_limits<float>::epsilon());
	float3 offset = float3(
		distance * (float)std::sin(phi) * (float)std::sin(theta),
		distance * (float)std::cos(phi),
		distance * (float)std::sin(phi) * (float)std::cos(theta));
	m_position += offset;

	// update view matrix


}

void Camera::UpdateProjectionMatrix()
{

}