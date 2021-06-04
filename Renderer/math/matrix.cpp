#include "matrix.h"
#include <memory>

Mat4x4::Mat4x4(float mat[4][4])
{
	std::memcpy(m, mat, 16 * sizeof(float));
}

Mat4x4::Mat4x4(
	float x00, float x01, float x02, float x03, 
	float x10, float x11, float x12, float x13, 
	float x20, float x21, float x22, float x23, 
	float x30, float x31, float x32, float x33)
{
	m[0][0] = x00;
	m[0][1] = x01;
	m[0][2] = x02;
	m[0][3] = x03;
	m[1][0] = x10;
	m[1][1] = x11;
	m[1][2] = x12;
	m[1][3] = x13;
	m[2][0] = x20;
	m[2][1] = x21;
	m[2][2] = x22;
	m[2][3] = x23;
	m[3][0] = x30;
	m[3][1] = x31;
	m[3][2] = x32;
	m[3][3] = x33;
}

void Mat4x4::SetRow(int index, const float3& v, float w /* = 1.0 */)
{
	m[index][0] = v.x;
	m[index][1] = v.y;
	m[index][2] = v.z;
	m[index][3] = w;
}

void Mat4x4::SetRow(int index, const float4& v)
{
	m[index][0] = v.x;
	m[index][1] = v.y;
	m[index][2] = v.z;
	m[index][3] = v.w;
}

void Mat4x4::SetCol(int index, const float3& v, float w /* = 1.0 */)
{
	m[0][index] = v.x;
	m[1][index] = v.y;
	m[2][index] = v.z;
	m[3][index] = w;
}

void Mat4x4::SetCol(int index, const float4& v)
{
	m[0][index] = v.x;
	m[1][index] = v.y;
	m[2][index] = v.z;
	m[3][index] = v.w;
}

float4 Mul(const float4& v, const float4x4& m)
{
	float4 x = float4(m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]) * v.x;
	float4 y = float4(m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]) * v.y;
	float4 z = float4(m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]) * v.z;
	float4 w = float4(m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]) * v.w;
	return x + y + z + w;
}

std::ostream& operator<<(std::ostream& os, const Mat4x4& m)
{

	return os;
}