#include "camera.h"
#include <iostream>

Camera::Camera(const float3& pos, const float3& target, float aspect)
	: m_position(pos), m_target(target), m_aspect(aspect)
{
	m_fov = ToRadians(90.f);
	m_up = float3(0.f, 1.f, 0.f);
	m_near = 0.01f;
	m_far = 10000.f;

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::SetFOV(float fovy)
{
	m_fov = ToRadians(fovy);
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

// width / height
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

float3 Camera::GetPosition() const
{
	return m_position;
}

float3 Camera::GetTarget() const
{
	return m_target;
}

float4x4 Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

float4x4 Camera::GetProjectionMatrix() const
{
	return m_projMatrix;
}

void Camera::Update(const float4& deltaCursor, const float deltaScroll)
{
	// update pan
	float3 look_at = m_target - m_position;
	float distance = look_at.Length();
	float factor = distance * (float)std::tan(m_fov * 0.5) * 2.f;
	float3 delta_x = m_right * deltaCursor.z * factor;
	float3 delta_y = m_up * deltaCursor.w * factor;

	// update orbit
	float theta = (float)std::atan2(-look_at.z, -look_at.x);
	float phi = (float)std::acos(-look_at.y / distance);
	factor = PI * 2.f;

	distance *= (float)std::pow(0.95, deltaScroll);
	theta -= deltaCursor.x * factor;
	phi -= deltaCursor.y * factor;
	phi = std::clamp<float>(phi, std::numeric_limits<float>::epsilon(), PI - std::numeric_limits<float>::epsilon());
	float3 offset = float3(
		distance * (float)std::sin(phi) * (float)std::cos(theta),
		distance * (float)std::cos(phi),
		distance * (float)std::sin(phi) * (float)std::sin(theta));

	m_target = m_target + delta_x + delta_y;
	m_position = m_target + offset;

	// update view matrix
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	float3 look_at = m_target - m_position;
	m_forward = Normalize(look_at);
	m_right = Cross(float3(0.f, 1.f, 0.f), m_forward);
	m_right = Normalize(m_right);
	m_up = Cross(m_forward, m_right);

	float x30 = -(m_position * m_right);
	float x31 = -(m_position * m_up);
	float x32 = -(m_position * m_forward);
	m_viewMatrix.SetCol(0, m_right, x30);
	m_viewMatrix.SetCol(1, m_up, x31);
	m_viewMatrix.SetCol(2, m_forward, x32);

}

void Camera::UpdateProjectionMatrix() 
{
	float Y = 1.f / std::tan(m_fov * 0.5);
	float X = Y / m_aspect;
	//float Z = m_near / (m_near - m_far);
	float Z = m_far / (m_far - m_near);
	//float W = -m_near * m_far / (m_near - m_far);
	float W = -m_near * m_far / (m_far - m_near);
	m_projMatrix.m[0][0] = X;
	m_projMatrix.m[1][1] = Y;
	m_projMatrix.m[2][2] = Z;
	m_projMatrix.m[2][3] = 1.f;
	m_projMatrix.m[3][2] = W;
	m_projMatrix.m[3][3] = 0.f;
}