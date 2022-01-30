#pragma once
#include <DirectXMath.h>

class Vector3:public DirectX::XMFLOAT3
{
public:
	//float x;
	//float y;
	//float z;

public:
	//�R���X�g���N�^
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(XMFLOAT3 v);
	//�����o�֐�
	float length() const;
	Vector3 &normalaize();
	float dot(const Vector3 &v)const;
	Vector3 cross(const Vector3 &v) const;

	//�P�����Z�q
	Vector3 operator+() const;
	Vector3 operator-() const;

	//������Z�q
	Vector3 &operator+=(const Vector3 &v);
	Vector3 &operator-=(const Vector3 &v);
	Vector3 &operator*=(float s);
	Vector3 &operator/=(float s);
	Vector3 &operator=(const XMFLOAT3 &v);
};

//2�����Z�q
const Vector3 operator+(const Vector3 &v1, const Vector3 &v2);
const Vector3 operator-(const Vector3 &v1, const Vector3 &v2);
const Vector3 operator*(const Vector3 &v, float s);
const Vector3 operator*(float s, const Vector3 &v);
const Vector3 operator/(const Vector3 &v, float s);
