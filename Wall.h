#pragma once
#include <DirectXMath.h>
#include "Object3D.h"
#include "Model.h"
#include "Camera.h"
#include "Collision.h"
#include "CollisionPrimitive.h"
#include <vector>
class Wall
{
public:
	Wall();
	~Wall();
	void Init(Camera &cam, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 collisionScale);
	void Update();
	void Draw();
	void Finalize();

	DirectX::XMFLOAT3 PushBack(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 size, DirectX::XMFLOAT3 MoveSpeed);

	bool IsHitWall(DirectX::XMFLOAT3 posA, DirectX::XMFLOAT3 posB);
private://静的メンバ変数
	static Model *wallModel;
public:
	static void SetModel(Model &model);
	std::vector<Triangle> &GetFaces();
	const DirectX::XMFLOAT3 &GetPos();
private:
	bool LineCollision(DirectX::XMFLOAT3 wallA, DirectX::XMFLOAT3 wallB, DirectX::XMFLOAT3 playerA, DirectX::XMFLOAT3 playerB);
	float LineCross(DirectX::XMFLOAT3 wallLine, DirectX::XMFLOAT3 playerLine);

	/// <summary>
	/// 壁の▽面の生成
	/// </summary>
	/// <param name="pos">壁の中心座標</param>
	/// <param name="scale">壁の描画拡大率</param>
	void SetFace(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale);
	/// <summary>
	/// 壁からの押し戻しの量を返す
	/// </summary>
	/// <param name="pos">プレイヤーの座標</param>
	/// <param name="size">半径</param>
	/// <param name="MoveSpeed"></param>
	/// <returns></returns>
	Object3D wallObj;
	DirectX::XMFLOAT3 collisionScale;
	Camera *cam;

	//四角面*4
	std::vector<Triangle> wallFaces;

public:
	Sphere sphere;
};
