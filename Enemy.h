#pragma once
#include"Model.h"
#include"Object3D.h"
#include<DirectXMath.h>
#include"Collision.h"
#include"EnemyBullet.h"
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

	const int TARGET_TIMER_END = 300;			//�v���C���[����������U������܂ł̑ҋ@����
	const int ATTACK_DELAY_TIMER_END = 200;		//�U����̍d������
	const int SEARCH_DELAY_TIMER_END = 300;		//���G�J�n������̃f�B���C�^�C�}�[(debug)
	const int MAX_SEARCH_TIMER = 100;			//���G�����E�؂�ւ��p�̃^�C�}�[

	const XMFLOAT3 TRIANGLE_UPPER_RIGHT_POS = { 50.0f,0.0f,100.0f };
	const XMFLOAT3 TRIANGLE_UPPER_LEFT_POS = { -50.0f,0.0f,100.0f };

public:
	/*---- �����o�ϐ� ----*/

	Object3D enemyData;		//�f�[�^
	XMFLOAT3 forwardVec;	//���ʃx�N�g��
	int status;				//Enemy�̏��
	XMMATRIX matRot;		//��]�s��
	EnemyBullet enemyBullet;//�G�̒e
	//�����蔻��p
	Ray forwardRay;			//���ʑ��̃��C
	Sphere sphere;			//�H�炢����p�̋�
	Triangle forwardTriangle;//���G�p���ʎO�p
	bool isAlive;			//�����t���O

	int HP;					//�q�b�g�|�C���g

	//���G�p
	int searchDelayTimer;	//���G�f�B���C�^�C�}�[
	int searchTimer;		//���G���鎞��
	int rotateStatus;		//���G���̃X�e�[�^�X

	//�^�[�Q�e�B���O�p
	int targetingTimer;
	XMFLOAT3 prevPlayerPos;	//�U����

	//�U���p
	int attackDelayTimer;	//�U����̍d������
	bool isAttack;			//�U���t���O

public:
	/*---- �����o�֐� ----*/

	//�R���X�g���N�^
	Enemy();

	//����������
	void Init(const Camera& cam);

	//��������
	void Generate(const Camera& cam, const XMFLOAT3& generatePos);

	//�X�V����
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam);

	//�`�揈��
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);

	//���G
	void Searching(const Sphere& playerSphere);

	//�^�[�Q�e�B���O
	void Targeting(const XMFLOAT3& playerPos);

	//�U��
	void Attack();

	//���ʃx�N�g���X�V
	void UpdateForwardVec(XMFLOAT3& forwardVec, XMMATRIX& matRot);

	//���S
	void Dead();

};

inline float Dot3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline float Cross2D(const XMFLOAT2& lhs, const XMFLOAT2& rhs) {
	return lhs.x * rhs.y - lhs.y * rhs.x;
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