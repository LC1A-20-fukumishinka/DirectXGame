#pragma once
#include"Model.h"
#include"Object3D.h"
#include<DirectXMath.h>
#include"Collision.h"

using namespace DirectX;

class EnemyBullet
{
public:
	/*---- �����o�萔 ----*/
	const float BULLET_SPEED = 10.0f;
	const float BULLET_RADIUS = 5.0f;
	const int MAX_DES_TIMER = 100;

	/*---- �����o�ϐ� ----*/
	Object3D bulletData;			//�e�̏��
	XMFLOAT3 forwardVec;			//�e�̐��ʃx�N�g��
	XMFLOAT3 moveVec;				//�e�̈ړ��x�N�g��
	bool isAlive;					//�����t���O
	Sphere bulletSphere;			//�����蔻��p�̋�
	int desTimer;					//�����邽�߂̃^�C�}�[

	/*---- �����o�֐� ----*/
	//�R���X�g���N�^
	EnemyBullet();

	//������
	void Init(const Camera& cam);

	//����
	void Generate(const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec);

	//�X�V
	void Update(const Camera& cam);;

	//���S
	void Dead();

	//�`��
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);
};