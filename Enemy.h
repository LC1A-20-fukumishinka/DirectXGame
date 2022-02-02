#pragma once
#include"Model.h"
#include"Object3D.h"
#include<DirectXMath.h>
#include"Collision.h"
#include"EnemyBullet.h"
#include"WallMgr.h"
#include "Sound.h"
//using XMFLOAT3 = DirectX::XMFLOAT3;
//using XMMATRIX = DirectX::XMMATRIX;
//using XMVECTOR = DirectX::XMVECTOR;
using namespace DirectX;

class Enemy
{
private:
	/*---- �萔 ----*/

	//�X�e�[�^�X
	enum STATUS
	{
		STATUS_SEARCH,			//���G
		STATUS_TARGET,			//�^�[�Q�e�B���O
		STATUS_ATTACK,			//�U��
		STATUS_TRACKING			//�ǔ�(��)
	};

	enum ROTATE_STATUS
	{
		ROTATE_STATUS_RIGHT,	//�E
		ROTATE_STATUS_LEFT,		//��
		ROTATE_STATUS_STOP		//��~
	};

	const int TARGET_TIMER_END = 100;			//�v���C���[����������U������܂ł̑ҋ@����
	const int ATTACK_DELAY_TIMER_END = 10;		//�U����̍d������
	const int SEARCH_DELAY_TIMER_END = 300;		//���G�J�n������̃f�B���C�^�C�}�[(debug)
	const int MAX_SEARCH_TIMER = 100;			//���G�����E�؂�ւ��p�̃^�C�}�[

	const XMFLOAT3 TRIANGLE_UPPER_RIGHT_POS = { 50.0f,0.0f,100.0f };
	const XMFLOAT3 TRIANGLE_UPPER_LEFT_POS = { -50.0f,0.0f,100.0f };

	const float ROTATE_SPEED = 8.0f;;	//��]���x
	const float ENEMY_RADIUS = 32.0f;

	const float TARGET_ROTATE_SPEED = 30.0f;

	static const int MAX_BULLET = 20;
	const int MAX_BULLET_TIMER = 10;			//�ˌ����[�g
	const float SEARCH_RADIUS = 150.0f;			//���G�͈�
	const float SENSING_RADIUS = 50.0f;			//���G���ɂ���ȏ�߂Â����犴�m�����`�͈�

public:
	/*---- �����o�ϐ� ----*/

	Object3D enemyData;		//�f�[�^
	XMFLOAT3 forwardVec;	//���ʃx�N�g��
	int status;				//Enemy�̏��
	XMMATRIX matRot;		//��]�s��
	EnemyBullet enemyBullet[MAX_BULLET];//�G�̒e
	//�����蔻��p
	Ray forwardRay;			//���ʑ��̃��C
	Sphere sphere;			//�H�炢����p�̋�
	Triangle forwardTriangle;//���G�p���ʎO�p
	bool isAlive;			//�����t���O
	int saveNum;
	bool isHit;

	int HP;					//�q�b�g�|�C���g

	//���G�p
	int searchDelayTimer;	//���G�f�B���C�^�C�}�[
	int searchTimer;		//���G���鎞��
	int rotateStatus;		//���G���̃X�e�[�^�X

	//�^�[�Q�e�B���O�p
	int targetingTimer;
	XMFLOAT3 prevPlayerPos;	//�U����

	XMFLOAT3 AttackVec;
	//�U���p
	int attackDelayTimer;	//�U����̍d������
	bool isAttack;			//�U���t���O
	int bulletTimer;		//�ˌ����[�g

	//�G�̎�ޔ��ʗp
	bool isEnemyTypeSeach;

	//�T�E���h
	Sound* targetSE;
	Sound* shotSE;

public:
	/*---- �����o�֐� ----*/

	//�R���X�g���N�^
	Enemy();

	//�f�X�g���N�^
	~Enemy();

	//����������
	void Init(const Camera& cam);

	//��������
	void Generate(const Camera& cam, const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec);

	//�X�V����
	void Update(const XMFLOAT3& playerPos, const float& angle, const bool& isAttack, const bool& isStop);

	//�e�̍X�V����
	void BulletUpdate();
	//�`�揈��
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);

	//���G
	void Searching(const XMFLOAT3& playerPos);

	//�^�[�Q�e�B���O
	void Targeting(const XMFLOAT3& playerPos);

	//�U��
	void Attack(const XMFLOAT3& playerPos);

	//���ʃx�N�g���X�V
	void UpdateForwardVec(XMFLOAT3& forwardVec, XMMATRIX& matRot);

	//���C�ƕǂ̓����蔻��
	bool CheckRay2Walls(const Ray& ray, std::vector<Wall>& walls, const XMFLOAT3& playerPos);

	XMFLOAT3 GetNearEnemyBulletPos(const XMFLOAT3& playerPos);

	//���S
	void Dead();

	//���ʃx�N�g���Z�b�g�p
	void SetForwardVec(const XMFLOAT3& forwardVec) { this->forwardVec = forwardVec; }

	Object3D* GetObj() { return &enemyData; }

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