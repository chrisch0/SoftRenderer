#pragma once
#include <initializer_list>
#include <cmath>
#include <ostream>
//#include <cassert>

template <typename T>
class Vec3;

template <typename T>
class Vec4;

template <typename T>
class Vec2
{
public:
	T x, y;
	Vec2() : x(0), y(0) {}
	explicit Vec2(T v) : x(v), y(v) {}
	Vec2(T x, T y) : x(x), y(y) {}
	explicit Vec2(const Vec3<T>& v) : x(v.x), y(v.y) {}
	explicit Vec2(const Vec4<T>& v) : x(v.x), y(v.y) {}
	~Vec2() {}

	Vec2(const Vec2<T>& v) : x(v.x), y(v.y) {}

	Vec2<T>& operator=(const Vec2<T>& rhs)
	{
		x = rhs.x; y = rhs.y;
		return *this;
	}

	Vec2<T> operator+(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x + rhs.x, y + rhs.y);
	}

	Vec2<T>& operator+=(const Vec2<T>& rhs)
	{
		x += rhs.x; y += rhs.y;
		return *this;
	}

	Vec2<T> operator-(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x - rhs.x, y - rhs.y);
	}

	Vec2<T> operator*(const T& s) const
	{
		return Vec2<T>(x * s, y * s);
	}

	Vec2<T>& operator*=(const T& s)
	{
		x *= s; y *= s;
		return *this;
	}

	Vec2<T> operator*(const Vec2<T>& s) const
	{
		return Vec2<T>(x * s.x, y * s.y);
	}

	Vec2<T> operator/(const T& s) const
	{
		return Vec2<T>(x / s, y / s);
	}

	Vec2<T> operator/(const Vec2<T>& rhs) const
	{
		return Vec2<T>(x / rhs.x, y / rhs.y);
	}

	Vec2<T> operator-() const
	{
		return Vec2<T>(-x, -y);
	}

	T operator[](uint8_t idx)
	{
		//assert(idx < 2);
		return this[idx];
	}

	bool operator<(const Vec2<T>& v) const
	{
		return x < v.x && x < v.y;
	}

	bool operator>(const Vec2<T>& v) const
	{
		return v < *this;
	}

	bool operator<=(const Vec2<T>& v) const 
	{
		return !(*this > v);
	}

	bool operator>=(const Vec2<T>& v) const
	{
		return !(*this < v);
	}

	

	T LengthSquared() const { return x * x + y * y; }
	T Length() const { return std::sqrt(x * x + y * y); }
};

using float2 = Vec2<float>;
using int2 = Vec2<int>;

template <typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	explicit Vec3(T v) : x(v), y(v), z(v) {}
	Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
	explicit Vec3(const Vec2<T>& v, T z) : x(v.x), y(v.y), z(z) {}
	explicit Vec3(const Vec4<T>& v) : x(v.x), y(v.y), z(v.z) {}
	~Vec3() {}

	Vec3<T>& operator=(const Vec3<T>& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
		return *this;
	}

	Vec3<T> operator+(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vec3<T>& operator+=(const Vec3<T>& rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}

	Vec3<T> operator+(const T t) const
	{
		return Vec3<T>(x + t, y + t, z + t);
	}

	Vec3<T> operator+=(const T t)
	{
		x += t; y += t; z += t;
		return *this;
	}

	Vec3<T> operator-(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vec3<T> operator*(const T s) const
	{
		return Vec3<T>(x * s, y * s, z * s);
	}

	Vec3<T>& operator*=(const T s)
	{
		x *= s; y *= s; z *= s;
		return *this;
	}

	Vec3<T> operator/(const T s) const
	{
		return Vec3<T>(x / s, y / s, z / s);
	}

	Vec3<T> operator-() const
	{
		return Vec3<T>(-x, -y, -z);
	}

	double LengthSquared() const { return x * x + y * y + z * z; }
	double Length() const { return std::sqrt(LengthSquared()); }
};

using float3 = Vec3<float>;

template <typename T>
class Vec4
{
public:
	T x, y, z, w;
	Vec4() : x(0), y(0), z(0), w(0) {}
	Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	Vec4(const Vec3<T>& v, T w = 0) : x(v.x), y(v.y), z(v.z), w(w) {}
	Vec4(const Vec2<T>& v0, const Vec2<T>& v1) : x(v0.x), y(v0.y), z(v1.x), w(v1.y) {}
	~Vec4() {};

	Vec4<T>& operator=(const Vec4<T>& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}

	Vec4<T> operator+(const Vec4<T>& rhs) const
	{
		return Vec4<T>(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	Vec4<T>& operator+=(const Vec4<T>& rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
		return *this;
	}

	Vec4<T> operator-(const Vec4<T>& rhs) const
	{
		return Vec4<T>(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	Vec4<T>& operator-=(const Vec4<T>& rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
		return *this;
	}

	Vec4<T> operator*(const T& s) const
	{
		return Vec4<T>(x * s, y * s, z * s, w * s);
	}

	Vec4<T>& operator*=(const T& s)
	{
		x *= s; y *= s; z *= s; w *= s;
		return *this;
	}

	Vec4<T> operator/(const T& s) const
	{
		return Vec4<T>(x / s, y / s, z / s, w / s);
	}

	Vec4<T> operator/=(const T& s) const
	{
		x /= s; y /= s; z /= s; w /= s;
		return *this;
	}

	Vec4<T> operator/(const Vec4<T>& s) const
	{
		return Vec4<T>(x / s.x, y / s.y, z / s.z, w / s.w);
	}

	Vec4<T> operator/=(const Vec4<T>& s) const
	{
		x /= s.x; y /= s.y; z /= s.z; w /= s.w;
		return *this;
	}

private:

};

using Color = Vec4<float>;
using float4 = Vec4<float>;

template <typename T>
inline Vec2<T> operator*(const T& s, const Vec2<T>& rhs)
{
	return rhs * s;
}

template <typename T>
Vec2<T> Lerp(const Vec2<T>& v1, const Vec2<T>& v2, T t)
{
	return v1 + (v2 - v1) * t;
}

template <typename T>
Vec2<T> Min(const Vec2<T>& v1, const Vec2<T>& v2)
{
	return Vec2<T>(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y);
}

template <typename T>
Vec2<T> Max(const Vec2<T>& v1, const Vec2<T>& v2)
{
	return Vec2<T>(
		v1.x > v2.x ? v1.x : v2.x,
		v1.y > v2.y ? v1.y : v2.y);
}

template <typename T>
Vec3<T> Min(const Vec3<T>& v1, const Vec3<T>& v2)
{
	return Vec3<T>(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y,
		v1.z < v2.z ? v1.z : v2.z);
}

template <typename T>
Vec3<T> Max(const Vec3<T>& v1, const Vec3<T> v2)
{
	return Vec3<T>(
		v1.x > v2.x ? v1.x : v2.x,
		v1.y > v2.y ? v1.y : v2.y,
		v1.z > v2.z ? v1.z : v2.z);
}

template <typename T>
inline Vec3<T> operator*(const T s, const Vec3<T>& rhs)
{
	return rhs * s;
}

template <typename T>
inline Vec3<T> operator+(const T s, const Vec3<T>& rhs)
{
	return rhs + s;
}

template <typename T>
Vec3<T> Mul(const Vec3<T>& v1, const Vec3<T>& v2)
{
	return Vec3<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

template <typename T>
T Dot(const Vec2<T>& v1, const Vec2<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

template <typename T>
T Dot(const Vec3<T>& v1, const Vec3<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
Vec3<T> Normalize(const Vec3<T>& v)
{
	return v / std::max<T>(v.Length(), 0.00001);
}

template <typename T>
Vec3<T> Cos(const Vec3<T>& v)
{
	return Vec3<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z));
}

template <typename T>
Vec3<T> Cross(const Vec3<T>& v1, const Vec3<T>& v2)
{
	double v1x = v1.x, v1y = v1.y, v1z = v1.z;
	double v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vec3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z),
		(v1x * v2y) - (v1y * v2x));
}

template <typename T>
Vec3<T> Lerp(const Vec3<T>& v1, const Vec3<T>& v2, T t)
{
	return v1 + (v2 - v1) * t;
}

template <typename T>
Vec4<T> Lerp(const Vec4<T>& v1, const Vec4<T>& v2, T t)
{
	return v1 + (v2 - v1) * t;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec2<T>& v)
{
	return os << "(" << v.x << ", " << v.y << ")";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T>& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec4<T>& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}