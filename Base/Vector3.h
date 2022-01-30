#pragma once
#include <DirectXMath.h>

class Vector3:public DirectX::XMFLOAT3
{
public:
	//float x;
	//float y;
	//float z;

public:
	//コンストラクタ
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(XMFLOAT3 v);
	//メンバ関数
	float length() const;
	Vector3 &normalaize();
	float dot(const Vector3 &v)const;
	Vector3 cross(const Vector3 &v) const;

	//単項演算子
	Vector3 operator+() const;
	Vector3 operator-() const;

	//代入演算子
	Vector3 &operator+=(const Vector3 &v);
	Vector3 &operator-=(const Vector3 &v);
	Vector3 &operator*=(float s);
	Vector3 &operator/=(float s);
	Vector3 &operator=(const XMFLOAT3 &v);
};

//2項演算子
const Vector3 operator+(const Vector3 &v1, const Vector3 &v2);
const Vector3 operator-(const Vector3 &v1, const Vector3 &v2);
const Vector3 operator*(const Vector3 &v, float s);
const Vector3 operator*(float s, const Vector3 &v);
const Vector3 operator/(const Vector3 &v, float s);
