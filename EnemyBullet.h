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
	const float GENERATE_POS = 30.0f;

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

inline float Dot3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline double Cross2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
	return (double)lhs.x * rhs.y - lhs.y * rhs.x;
}

inline XMFLOAT3 Cross3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return XMFLOAT3(lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x);
}

inline float Length3D(const XMFLOAT3& rhs) {
	return sqrtf(Dot3D(rhs, rhs));
}

inline XMFLOAT3 Normalize3D(const XMFLOAT3& rhs) {
	float len = Length3D(rhs);
	XMFLOAT3 buff = rhs;
	buff.x /= len;
	buff.y /= len;
	buff.z /= len;
	return buff;
}

inline int GetRand(const int& min, const int& max) {
	return (rand() % (max - min + 1)) + min;
}

inline XMFLOAT3 AddXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline XMFLOAT3 MulXMFLOAT3(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return XMFLOAT3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

//3D���W���ł̓�_�Ԃ̋��������߂�
inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
}

inline float calAngle(const XMFLOAT3& rhs, const XMFLOAT3& lhs)
{
	return Dot3D(lhs, rhs) / (Length3D(lhs) * Length3D(rhs));
}