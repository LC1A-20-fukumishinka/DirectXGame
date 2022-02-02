#pragma once
#include "Input/DirectInput.h"
#include "3D/Model.h"
#include "3D/Object3D.h"
#include "ModelPipeline.h"
#include "TextureMgr.h"
#include "Sprite.h"
#include <DirectXMath.h>
#include <math.h>
#include "Vector3.h"
#include "particleManager.h"
#include "Sound.h"

const int INVINCIBLE_COUNT = 60;					//���G����
const int STOP_TIME_COUNT = 300;						//�ő厞�Ԓ�~��
const int STOP_TIME_DELAY = 30;						//�U���\�܂ł̃N�[���^�C��
const int ATTACK_DELAY = 30;						//�U���̍ő�CT
const int DASH_DELAY = 30;							//�_�b�V��CT
const int MAX_HP = 3;								//HP
const float MOVE_SPEED = 2.5f;						//��������
const float DASH_SPEED = 65.0f;						//�_�b�V��
const float MOVE_ANGLE = 10.0f;						//������ς��鑬��
const float ATTACK_ANGLE = 90;						//�U���͈͊p�x

using namespace DirectX;

class Player
{
private:
	Input* input = Input::GetInstance();
	Model model;
	Object3D obj;
	Camera cam;

private:
	XMFLOAT3 pos;									//�|�W�V����
	XMFLOAT3 vec3;									//�����Ă������
	Vector3 nextCameraPos;
	//XMFLOAT3 contVec3;
	int hp;											//�q�b�g�|�C���g
	int stopTimeCount;								//���Ԃ��~�߂Ă���Ԃ̃J�E���g
	int stopTImeDelay;								//���Ԓ�~��CT
	int attackCount;								//�U���̃N�[���^�C��
	int attackDelay;								//�U����CT
	int drawCount;									//��_���G�t�F�N�g�p
	int damagedCount;								//���G���ԊǗ�
	int dashDelay;									//DASH��CT
	float angle;									//�ړ�����p�x
	float easeTimer;								//�C�[�W���O
	float movePower;								//�ړ��̊����p
	bool attackFlag;								//�U�����Ă��邩
	bool stopTimeFlag;								//���Ԏ~�߂Ă��邩
	bool isHit;										//�U��������������
	bool isDead;									//HP��0�ɂȂ�����
	bool isDamaged;									//�������_���[�W���󂯂���
	bool isClear;									//�N���A������
	bool isDash;									//�_�b�V��������

private:
	int GH1;
	int GH2;
	int partGH;
	Sprite dead;
	Sprite clear;
	bool spriteDeadFlag;							//Update��Draw�ɓ`����p
	bool spriteClearFlag;							//Update��Draw�ɓ`����p
	bool isEffect;									//�G�t�F�N�g�����ۂ�(true->�G�t�F�N�g��)
	ParticleManager shift;

	Vector3 cameraToPlayer;
	Sound* damageSE;
	Sound* ShiftSE;
	Sound* StopSE;
	Sound* PlaySE;
	Sound* AttackSE;
	//Sound* damageSE;

public:
	Player(int deadGraph, int clearGraph, int particle, int DamageSound);
	~Player();
	void Init(const Camera& camera, const XMFLOAT3& pos);
	void Input(const Camera& camera);
	void Update(Camera& camera, const XMFLOAT3& enemyPos);
	void Draw(const PipeClass::PipelineSet& pipelineSet);
	void Finalize();
	void PushBack(const XMFLOAT3& enemyPos);
	void DeathEffect(Camera& camera);
	void ClearEffect(Camera& camera, bool setGoalAndCheak);
	bool SetGoalAndCheak(const XMFLOAT3& lowerLeft, const XMFLOAT3& upperRight);	//�����ƉE����w�肷��

public:
	XMFLOAT3 GetPos() { return pos; }									//�|�W�V������Ԃ�
	XMFLOAT3 GetCameraToPlayer() { return cameraToPlayer; };
	//XMFLOAT3 GetDirection() { return direction; }						//�����Ă������Ԃ�
	XMFLOAT3 GetVec3() { return vec3; }									//�����x�N�g����Ԃ�
	void SetVec3(XMFLOAT3 vec3) { this->vec3 = vec3; }					//�����x�N�g�����Z�b�g
	void SetPos(XMFLOAT3 pos) { this->pos = pos; }

	void Damaged();


	int GetHP() { return hp; }											//HP��Ԃ�
	int GetStopTimeCount() { return stopTimeCount; }
	int GetStopTimeDelay() { return stopTImeDelay; }
	float GetAngle() { return angle; }
	bool GetAttackFlag() { return attackFlag; }							//�U�����Ă��邩��Ԃ�
	bool GetStopTimeFlag() { return stopTimeFlag; }						//���Ԃ��~�߂Ă��邩�~�߂Ă��Ȃ�����Ԃ�
	bool IsHit() { return isHit; }										//�G�ɍU��������������
	bool IsDead() { return isDead; }									//���@�����񂾂�
	bool IsEffect() { return isEffect; }								//�G�t�F�N�g���I�������(������false������Ǘ��v����)
	bool IsClear() { return isClear; }									//�N���A������
	bool IsDash() { return isDash; }									//�_�b�V��������
	Object3D* GetObj() { return &obj; }									//���f�������擾
	Model* GetModel() { return &model; }

private:
	void ConvertToRadian(float& degree)
	{
		degree = degree / 180.0f * XM_PI;
	}
	void ConvertToDegree(float& radian)
	{
		radian = radian * 180.0f / XM_PI;
	}

public:
	float easeOutCubic(float t) {
		return 1 - powf(1 - t, 3);
	}

	float easeInOutSine(float t)
	{
		//return -(cos(XM_PI * t) - 1) / 2;
		return t < 0.5 ? 16 * t * t * t * t * t : 1 - pow(-2 * t + 2, 5) / 2;
	}

private:
	float RotateEarliestArc(float NowAngle, float EndAngle)
	{
		if (fabsf(EndAngle - NowAngle) > 180.0f) {
			if (NowAngle < 180.0f) {
				NowAngle += 360.0f;
			}
			else {
				NowAngle -= 360.0f;
			}
		}
		return EndAngle - NowAngle;
	}
};

