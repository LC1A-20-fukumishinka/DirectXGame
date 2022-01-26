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
private://�ÓI�����o�ϐ�
	static Model *wallModel;
public:
	static void SetModel(Model &model);
	std::vector<Triangle> &GetFaces();
	const DirectX::XMFLOAT3 &GetPos();
private:
	bool LineCollision(DirectX::XMFLOAT3 wallA, DirectX::XMFLOAT3 wallB, DirectX::XMFLOAT3 playerA, DirectX::XMFLOAT3 playerB);
	float LineCross(DirectX::XMFLOAT3 wallLine, DirectX::XMFLOAT3 playerLine);

	/// <summary>
	/// �ǂ́��ʂ̐���
	/// </summary>
	/// <param name="pos">�ǂ̒��S���W</param>
	/// <param name="scale">�ǂ̕`��g�嗦</param>
	void SetFace(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale);
	/// <summary>
	/// �ǂ���̉����߂��̗ʂ�Ԃ�
	/// </summary>
	/// <param name="pos">�v���C���[�̍��W</param>
	/// <param name="size">���a</param>
	/// <param name="MoveSpeed"></param>
	/// <returns></returns>
	Object3D wallObj;
	DirectX::XMFLOAT3 collisionScale;
	Camera *cam;

	//�l�p��*4
	std::vector<Triangle> wallFaces;

public:
	Sphere sphere;
};
