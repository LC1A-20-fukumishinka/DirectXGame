#pragma once
#include "Input/DirectInput.h"
#include "3D/Model.h"
#include "3D/Object3D.h"
#include "ModelPipeline.h"

#include <DirectXMath.h>
#include <math.h>

const int STOP_TIME_COUNT = 60;						//最大時間停止量
const int STOP_TIME_DELAY = 30;						//攻撃可能までのクールタイム
const int ATTACK_DELAY = 30;						//攻撃の最大CT
const int MAX_HP = 3;								//HP
const float MOVE_SPEED = 1.0f;						//動く速さ
const float MOVE_ANGLE = 3.0f;						//向きを変える速さ
const float ATTACK_ANGLE = 30;						//攻撃範囲角度

using namespace DirectX;

class Player
{
private:
	Input* input = Input::GetInstance();
	Model model;
	Object3D obj;
	Camera cam;

private:
	XMFLOAT3 pos;									//ポジション
	XMFLOAT3 direction;								//向いている方向
	XMFLOAT3 vec3;
	int hp;											//ヒットポイント
	int stopTimeCount;								//時間を止めている間のカウント
	int stopTImeDelay;								//時間停止のCT
	int attackCount;								//攻撃のクールタイム
	int attackDelay;								//攻撃のCT
	float angle;									//移動する角度
	bool attackFlag;								//攻撃しているか
	bool stopTimeFlag;								//時間止めているか
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
	XMFLOAT3 GetPos() { return pos; }									//ポジションを返す
	XMFLOAT3 GetDirection() { return direction; }						//向いてる方向を返す
	XMFLOAT3 GetVec3() { return vec3; }									//方向ベクトルを返す
	void SetVec3(XMFLOAT3 vec3) { this->vec3 = vec3; }					//方向ベクトルをセット
	void Damaged() { if (hp > 0) { hp--; } else { isDead = true; } }	//HPを減らす
	int GetHP() { return hp; }											//HPを返す
	//float GetSpeed() { return MOVE_SPEED; }
	bool GetAttackFlag() { return attackFlag; }							//攻撃しているかを返す
	bool GetStopTimeFlag() { return stopTimeFlag; }						//時間を止めているか止めていないかを返す
	bool IsHit() { return isHit; }										//敵に攻撃が当たったか
	bool IsDead() { return isDead; }									//自機が死んだか

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

