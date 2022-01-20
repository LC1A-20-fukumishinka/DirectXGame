#pragma once
#include <DirectXMath.h>
#include "Object3D.h"
#include "Model.h"
#include "Camera.h"
#include"Collision.h"
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
private://�ÓI�����o�ϐ�
	static Model *wallModel;
public:
	static void SetModel(Model &model);
private:
	bool LineCollision(DirectX::XMFLOAT3 wallA, DirectX::XMFLOAT3 wallB, DirectX::XMFLOAT3 playerA, DirectX::XMFLOAT3 playerB);
	float LineCross(DirectX::XMFLOAT3 wallLine, DirectX::XMFLOAT3 playerLine);
	/// <summary>
	/// �ǂ���̉����߂��̗ʂ�Ԃ�
	/// </summary>
	/// <param name="pos">�v���C���[�̍��W</param>
	/// <param name="size">���a</param>
	/// <param name="MoveSpeed"></param>
	/// <returns></returns>
	DirectX::XMFLOAT3 collisionScale;
	Camera *cam;
public:
	Object3D wallObj;
	Sphere sphere;
};
