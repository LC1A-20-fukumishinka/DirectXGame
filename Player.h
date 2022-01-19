#pragma once
#include "Input/DirectInput.h"
#include "3D/Model.h"
#include "3D/Object3D.h"
#include "ModelPipeline.h"

#include <DirectXMath.h>
#include <math.h>

const int STOP_TIME_COUNT = 60;						//�ő厞�Ԓ�~��
const int STOP_TIME_DELAY = 30;						//�U���\�܂ł̃N�[���^�C��
const int ATTACK_DELAY = 30;						//�U���̍ő�CT
const int MAX_HP = 3;								//HP
const float MOVE_SPEED = 1.0f;						//��������
const float MOVE_ANGLE = 3.0f;						//������ς��鑬��
const float ATTACK_ANGLE = 30;						//�U���͈͊p�x

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
	XMFLOAT3 direction;								//�����Ă������
	XMFLOAT3 vec3;
	int hp;											//�q�b�g�|�C���g
	int stopTimeCount;								//���Ԃ��~�߂Ă���Ԃ̃J�E���g
	int stopTImeDelay;								//���Ԓ�~��CT
	int attackCount;								//�U���̃N�[���^�C��
	int attackDelay;								//�U����CT
	float angle;									//�ړ�����p�x
	bool attackFlag;								//�U�����Ă��邩
	bool stopTimeFlag;								//���Ԏ~�߂Ă��邩
	bool isHit;
	bool isDead;

public:
	Player();
	~Player();
	void Init(const Camera& camera);
	void Input(Camera& camera);
	void Update(Camera& camera, const XMFLOAT3& enemyPos);
	void Draw(const PipeClass::PipelineSet& pipelineSet);
	void Finalize();

public:
	XMFLOAT3 GetPos() { return pos; }									//�|�W�V������Ԃ�
	XMFLOAT3 GetDirection() { return direction; }						//�����Ă������Ԃ�
	XMFLOAT3 GetVec3() { return vec3; }									//�����x�N�g����Ԃ�
	void SetVec3(XMFLOAT3 vec3) { this->vec3 = vec3; }					//�����x�N�g�����Z�b�g
	void Damaged() { if (hp > 0) { hp--; } else { isDead = true; } }	//HP�����炷
	int GetHP() { return hp; }											//HP��Ԃ�
	//float GetSpeed() { return MOVE_SPEED; }
	bool GetAttackFlag() { return attackFlag; }							//�U�����Ă��邩��Ԃ�
	bool GetStopTimeFlag() { return stopTimeFlag; }						//���Ԃ��~�߂Ă��邩�~�߂Ă��Ȃ�����Ԃ�
	bool IsHit() { return isHit; }										//�G�ɍU��������������
	bool IsDead() { return isDead; }									//���@�����񂾂�

private:
	void ConvertToRadian(float& degree)
	{
		degree = degree / 180.0f * XM_PI;
	}
	void ConvertToDegree(float& radian)
	{
		radian = radian * 180.0f / XM_PI;
	}
};

