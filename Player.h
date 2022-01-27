#pragma once
#include "Input/DirectInput.h"
#include "3D/Model.h"
#include "3D/Object3D.h"
#include "ModelPipeline.h"
#include "TextureMgr.h"
#include "Sprite.h"

#include <DirectXMath.h>
#include <math.h>

const int STOP_TIME_COUNT = 360;						//最大時間停止量
const int STOP_TIME_DELAY = 30;						//攻撃可能までのクールタイム
const int ATTACK_DELAY = 30;						//攻撃の最大CT
const int MAX_HP = 3;								//HP
const float MOVE_SPEED = 1.5f;						//動く速さ
const float MOVE_ANGLE = 7.0f;						//向きを変える速さ
const float ATTACK_ANGLE = 90;						//攻撃範囲角度

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
	XMFLOAT3 vec3;									//向いている方向
	XMFLOAT3 contVec3;
	int hp;											//ヒットポイント
	int stopTimeCount;								//時間を止めている間のカウント
	int stopTImeDelay;								//時間停止のCT
	int attackCount;								//攻撃のクールタイム
	int attackDelay;								//攻撃のCT
	int drawCount;									//被ダメエフェクト用
	float angle;									//移動する角度
	float easeTimer;								//イージング
	bool attackFlag;								//攻撃しているか
	bool stopTimeFlag;								//時間止めているか
	bool isHit;										//攻撃が当たったか
	bool isDead;									//HPが0になったか
	bool isDamaged;									//自分がダメージを受けたか
	bool isOverTrigger;

private:
	int GH1;
	int GH2;
	Sprite dead;
	Sprite clear;
	bool spriteDeadFlag;							//UpdateとDrawに伝える用
	bool spriteClearFlag;							//UpdateとDrawに伝える用
	bool isEffect;									//エフェクト中か否か(true->エフェクト中)

public:
	Player();
	~Player();
	void Init(const Camera& camera, const XMFLOAT3& pos);
	void Input(const Camera& camera);
	void Update(Camera& camera, const XMFLOAT3& enemyPos);
	void Draw(const PipeClass::PipelineSet& pipelineSet);
	void Finalize();
	void PushBack(const XMFLOAT3& enemyPos);
	void DeathEffect(Camera& camera);
	void ClearEffect(Camera& camera, bool setGoalAndCheak);
	bool SetGoalAndCheak(const XMFLOAT3& lowerLeft, const XMFLOAT3& upperRight);	//左下と右上を指定する

public:
	XMFLOAT3 GetPos() { return pos; }									//ポジションを返す
	//XMFLOAT3 GetDirection() { return direction; }						//向いてる方向を返す
	XMFLOAT3 GetVec3() { return vec3; }									//方向ベクトルを返す
	void SetVec3(XMFLOAT3 vec3) { this->vec3 = vec3; }					//方向ベクトルをセット
	void SetPos(XMFLOAT3 pos) { this->pos = pos; }

	void Damaged()
	{
		isDamaged = true;
		if (hp > 0) { hp--; }
		if (hp <= 0) {
			isDead = true;
		}
	}	//HPを減らす

	int GetHP() { return hp; }											//HPを返す
	int GetStopTimeCount() { return stopTimeCount; }
	int GetStopTimeDelay() { return stopTImeDelay; }
	float GetAngle() { return angle; }
	bool GetAttackFlag() { return attackFlag; }							//攻撃しているかを返す
	bool GetStopTimeFlag() { return stopTimeFlag; }						//時間を止めているか止めていないかを返す
	bool IsHit() { return isHit; }										//敵に攻撃が当たったか
	bool IsDead() { return isDead; }									//自機が死んだか
	bool IsEffect() { return isEffect; }

private:
	void ConvertToRadian(float& degree)
	{
		degree = degree / 180.0f * XM_PI;
	}
	void ConvertToDegree(float& radian)
	{
		radian = radian * 180.0f / XM_PI;
	}
	float easeOutCubic(float t) {
		return 1 - powf(1 - t, 3);
	}
};

