#pragma once
#include"Model.h"
#include"Object3D.h"
#include<DirectXMath.h>
#include"Collision.h"
#include "BombEffect.h"
using namespace DirectX;

class EnemyBullet
{
public:
	/*---- �����o�萔 ----*/
	const float BULLET_SPEED = 8.0f;
	const float BULLET_SPEED_FAST = 50.0f;
	const float BULLET_RADIUS = 5.0f;
	const int MAX_DES_TIMER = 100;
	const float EXPLOSION_RADIUS = 40.0f;
	const int MAX_EXPLOSION_TIMER = 60;

	enum BULLET_STATUS
	{
		BULLET_STATUS_ALIVE,			//����
		BULLET_STATUS_EXPLOSION,		//����
	};

	/*---- �����o�ϐ� ----*/
	Object3D bulletData;			//�e�̏��
	XMFLOAT3 forwardVec;			//�e�̐��ʃx�N�g��
	XMFLOAT3 moveVec;				//�e�̈ړ��x�N�g��
	float speed;					//�e�̑��x
	bool isAlive;					//�����t���O
	Sphere bulletSphere;			//�����蔻��p�̋�
	int desTimer;					//�����邽�߂̃^�C�}�[
	int status;						//�e�̏��
	int explosionTimer;				//���������^�C�}�[
	BombEffect effect;
	/*---- �����o�֐� ----*/
	//�R���X�g���N�^
	EnemyBullet();

	//������
	void Init(const Camera& cam);

	//����
	void Generate(const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec, const bool& isEnemyTypeSearch);

	//�X�V
	void Update();;

	//����
	void Explosion();

	//���S
	void Dead();

	//�`��
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);
};