#pragma once
#include "math/math.h"

class Camera
{
public:
	Camera();
	Camera(const float3& pos, const float3& target, float aspect, bool isPerspective = true);
	~Camera() {}

	void SetFOV(float fovyAngle);
	void SetNear(float near);
	void SetFar(float far);
	void SetAspect(float aspect);
	void SetPosition(const float3& pos);
	void SetTarget(const float3& target);
	void SetWidth(float width);
	void SetHeight(float height);
	void SetCameraType(bool isPerspective);
	float3 GetPosition() const;
	float3 GetTarget() const;
	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	void Update(const float4& deltaCursor, const float deltaScroll);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

private:
	void UpdatePerspectiveProMat();
	void UpdateOrthographicProMat();

	float3 m_position;
	float3 m_target;
	float m_near;
	float m_far;
	float m_fov;
	float m_aspect;
	float3 m_up;
	float3 m_forward;
	float3 m_right;
	float4x4 m_viewMatrix;
	float4x4 m_projMatrix;
	bool m_isPerspective;
	float m_width;
	float m_height;
};