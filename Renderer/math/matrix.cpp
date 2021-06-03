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

}

void Mat4x4::SetRow(int index, const float4& v)
{

}