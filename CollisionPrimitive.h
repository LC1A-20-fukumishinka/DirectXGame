#pragma once

#include <DirectXMath.h>

struct Sphere
{
	//中心座標
	DirectX::XMVECTOR center = { 0.0f,0.0f ,0.0f ,1.0f };
	//半径
	float radius = 1.0f;
};

struct Plane
{
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0.0f,1.0f ,0.0f ,0.0f };
	//原点(0.0f,0.0f,0.0f)からの距離
	float distance = 0.0f;
};

class Triangle
{
public:
	//頂点座標3つ
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	//法線ベクトル
	DirectX::XMVECTOR normal;

	/// <summary>
	/// 法線の計算
	/// </summary>
	void ComputeNormal();
};

struct Ray
{
	//始点座標
	DirectX::XMVECTOR start = { 0.0f, 0.0f, 0.0f, 1.0f };
	//方向
	DirectX::XMVECTOR dir = { 1.0f, 0.0f ,0.0f ,0.0f };
};

struct Box
{
	//上(Top), 左(Left), 近(Near)
	DirectX::XMVECTOR TLN;

	//下(Bottom), 右(Right), 遠(Near)
	DirectX::XMVECTOR BRF;
};