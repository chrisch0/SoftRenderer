#pragma once
#include "vec.h"

class Mat4x4
{
public:
	float m[4][4];

	Mat4x4()
	{
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
		m[0][1] = m[0][2] = m[0][3] = 
			m[1][0] = m[1][2] = m[1][3] =
			m[2][0] = m[2][1] = m[2][3] =
			m[3][0] = m[3][1] = m[3][2] = 0.f;
	}
	Mat4x4(float mat[4][4]);
	Mat4x4(
		float x00, float x01, float x02, float x03,
		float x10, float x11, float x12, float x13,
		float x20, float x21, float x22, float x23,
		float x30, float x31, float x32, float x33
		);
	~Mat4x4() {};

	Mat4x4 operator*(const Mat4x4& rhs)
	{
		Mat4x4 r;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				r.m[i][j] =
					m[i][0] * rhs.m[0][j] +
					m[i][1] * rhs.m[1][j] +
					m[i][2] * rhs.m[2][j] +
					m[i][3] * rhs.m[3][j];
			}
		}
		return r;
	}

	void SetRow(int index, const float3& v, float w = 1.0);
	void SetRow(int index, const float4& v);
};

using float4x4 = Mat4x4;