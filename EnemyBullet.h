#pragma once
#include"Model.h"
#include"Object3D.h"
#include<DirectXMath.h>
#include"Collision.h"

using namespace DirectX;

class EnemyBullet
{
public:
	/*---- メンバ定数 ----*/
	const float BULLET_SPEED = 10.0f;
	const float BULLET_RADIUS = 5.0f;
	const int MAX_DES_TIMER = 100;

	/*---- メンバ変数 ----*/
	Object3D bulletData;			//弾の情報
	XMFLOAT3 forwardVec;			//弾の正面ベクトル
	XMFLOAT3 moveVec;				//弾の移動ベクトル
	bool isAlive;					//生存フラグ
	Sphere bulletSphere;			//当たり判定用の球
	int desTimer;					//消えるためのタイマー

	/*---- メンバ関数 ----*/
	//コンストラクタ
	EnemyBullet();

	//初期化
	void Init(const Camera& cam);

	//生成
	void Generate(const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec);

	//更新
	void Update(const Camera& cam);;

	//死亡
	void Dead();

	//描画
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);
};