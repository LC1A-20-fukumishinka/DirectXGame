#include "Vector3.h"
#include <cmath>

const Vector3 operator+(const Vector3 &v1, const Vector3 &v2)
{
	Vector3 tmp(v1);
	return tmp += v2;
}

const Vector3 operator-(const Vector3 &v1, const Vector3 &v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3 &v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3 &v)
{
	return v * s;
}

const Vector3 operator/(const Vector3 &v, float s)
{
	Vector3 temp(v);
	return v * (1 / s);
}

Vector3::Vector3() :
	Vector3(0, 0, 0)
{
}

Vector3::Vector3(float x, float y, float z) :
	DirectX::XMFLOAT3{ x,y,z }
{
}

Vector3::Vector3(XMFLOAT3 v):
Vector3(v.x, v.y, v.z)
{

}

float Vector3::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

Vector3 &Vector3::normalaize()
{
	float l = length();
	if (l <= 0)
	{
		return *this;
	}
	return *this /= l;
}

float Vector3::dot(const Vector3 &v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::cross(const Vector3 &v) const
{
	return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3 &Vector3::operator+=(const Vector3 &v)
{
	x = x + v.x;
	y = y + v.y;
	z = z + v.z;
	return *this;
}

Vector3 &Vector3::operator-=(const Vector3 &v)
{
	x = x - v.x;
	y = y - v.y;
	z = z - v.z;
	return *this;
}

Vector3 &Vector3::operator*=(float s)
{
	x = x * s;
	y = y * s;
	z = z * s;
	return *this;
}

Vector3 &Vector3::operator/=(float s)
{
	x = x / s;
	y = y / s;
	z = z / s;
	return *this;
}

Vector3 &Vector3::operator=(const XMFLOAT3 &v)
{

	x = v.x; 
	y = v.y;
	z = v.z;
	return *this;
}
