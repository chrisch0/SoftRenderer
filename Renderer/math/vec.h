#pragma once
#include <initializer_list>
#include <cmath>
#include <ostream>

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
	Vec2(T v) : x(v), y(v) {}
	Vec2(T x, T y) : x(x), y(y) {}
	Vec2(const Vec3<T>& v) : x(v.x), y(v.y) {}
	Vec2(const Vec4<T>& v) : x(v.x), y(v.y) {}
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

	T LengthSquared() const { return x * x + y * y; }
	T Length() const { return std::sqrt(x * x + y * y); }
};

using vec2d = Vec2<double>;
using vec2f = Vec2<float>;
using vec2i = Vec2<int>;

template <typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T v) : x(v), y(v), z(v) {}
	Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
	Vec3(const Vec2<T>& v, T z) : x(v.x), y(v.y), z(z) {}
	Vec3(const Vec4<T>& v) : x(v.x), y(v.y), z(v.z) {}
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
	Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	Vec4(const Vec3<T>& v, T w = 0) : x(v.x), y(v.y), z(v.z), w(w) {}
	Vec4(const Vec2<T>& v0, const Vec2<T>& v1) : x(v0.x), y(v0.y), z(v1.x), w(v1.y) {}
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
inline Vec3<T> operator+(const T& s, const Vec3<T>& rhs)
{
	return rhs + s;
}

template <typename T>
Vec3<T> normalize(const Vec3<T>& v)
{
	return v / v.Length();
}

template <typename T>
Vec3<T> cos(const Vec3<T>& v)
{
	return Vec3<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z));
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