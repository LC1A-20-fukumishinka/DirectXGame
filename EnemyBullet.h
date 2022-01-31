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
	/*---- メンバ定数 ----*/
	const float BULLET_SPEED = 8.0f;
	const float BULLET_SPEED_FAST = 50.0f;
	const float BULLET_RADIUS = 5.0f;
	const int MAX_DES_TIMER = 100;
	const float EXPLOSION_RADIUS = 40.0f;
	const int MAX_EXPLOSION_TIMER = 60;

	enum BULLET_STATUS
	{
		BULLET_STATUS_ALIVE,			//生存
		BULLET_STATUS_EXPLOSION,		//爆発
	};

	/*---- メンバ変数 ----*/
	Object3D bulletData;			//弾の情報
	XMFLOAT3 forwardVec;			//弾の正面ベクトル
	XMFLOAT3 moveVec;				//弾の移動ベクトル
	float speed;					//弾の速度
	bool isAlive;					//生存フラグ
	Sphere bulletSphere;			//当たり判定用の球
	int desTimer;					//消えるためのタイマー
	int status;						//弾の状態
	int explosionTimer;				//爆発持続タイマー
	BombEffect effect;
	/*---- メンバ関数 ----*/
	//コンストラクタ
	EnemyBullet();

	//初期化
	void Init(const Camera& cam);

	//生成
	void Generate(const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec, const bool& isEnemyTypeSearch);

	//更新
	void Update();;

	//爆発
	void Explosion();

	//死亡
	void Dead();

	//描画
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);
};