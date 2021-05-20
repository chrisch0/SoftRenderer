#pragma once
#include <initializer_list>
#include <cmath>

template <typename T>
class Vec2
{
public:
	T x, y;
	Vec2() : x(0), y(0) {}
	Vec2(T v) : x(v), y(v) {}
	Vec2(T x, T y) : x(x), y(y) {}
	~Vec2() {}

	Vec2(const Vec2<T>& v) : x(v.x), y(v.y) {}

	Vec2<T>& operator=(const Vec2<T>& rhs)
	{
		x = rhs.x; y = rhs.y;
		return *this;
	}

	Vec2<T>& operator=(const std::initializer_list<T>& il)
	{
		assert(il.size() >= 2);
		x = il[0]; y = il[1];
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

	double operator*(const Vec2<T>& rhs) const
	{
		return x * rhs.x + y * rhs.y;
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

	double LengthSquared() const { return x * x + y * y; }
	double Length() const { return std::sqrt(x * x + y * y); }
};

using Vec2d = Vec2<double>;
using vec2f = Vec2<float>;

template <typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T v) : x(x), y(v), z(v) {}
	Vec3(T x, T y, T z) : x(x), y(x), z(x) {};
	~Vec3() {}

	Vec3<T>& operator=(const Vec3<T>& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
		return *this;
	}

	Vec3<T>& operator=(const std::initializer_list<T>& il)
	{
		assert(il.size() >= 3);
		x = il[0]; y = il[1]; z = il[2];
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

	Vec3<T> operator-(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vec3<T> operator*(const T& s) const
	{
		return Vec3<T>(x * s, y * s, z * s);
	}

	Vec3<T>& operator*=(const T& s)
	{
		x *= s; y *= s; z *= s;
		return *this;
	}

	double operator*(const Vec3<T>& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vec3<T> operator/(const T& s) const
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

using vec3f = Vec3<float>;

template <typename T>
class Vec4
{
public:
	T x, y, z, w;
	Vec4() : x(0), y(0), z(0), w(0) {}
	Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};
	~Vec4() {};

	Vec4<T>& operator=(const Vec4<T>& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}

	Vec4<T>& operator=(const std::initializer_list<T>& il)
	{
		assert(il.size() >= 4);
		x = il[0]; y = il[1]; z = il[2]; w = il[3];
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
private:

};

using Color = Vec4<unsigned char>;
using vec4f = Vec4<float>;

template <typename T>
inline Vec2<T> operator*(const T& s, const Vec2<T>& rhs)
{
	return rhs * s;
}

template <typename T>
inline Vec3<T> operator*(const T& s, const Vec3<T>& rhs)
{
	return rhs * s;
}

template <typename T>
Vec3<T> normalize(const Vec3<T>& v)
{
	return v / v.Length();
}

