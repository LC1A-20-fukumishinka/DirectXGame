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
	/*---- 定数 ----*/

	//ステータス
	enum STATUS
	{
		STATUS_SEARCH,			//索敵
		STATUS_TARGET,			//ターゲティング
		STATUS_ATTACK,			//攻撃
		STATUS_TRACKING			//追尾(仮)
	};

	enum ROTATE_STATUS
	{
		ROTATE_STATUS_RIGHT,	//右
		ROTATE_STATUS_LEFT,		//左
		ROTATE_STATUS_STOP		//停止
	};

	const int TARGET_TIMER_END = 300;			//プレイヤーを見つけた後攻撃するまでの待機時間
	const int ATTACK_DELAY_TIMER_END = 200;		//攻撃後の硬直時間
	const int SEARCH_DELAY_TIMER_END = 300;		//索敵開始した後のディレイタイマー(debug)
	const int MAX_SEARCH_TIMER = 100;			//索敵時左右切り替え用のタイマー

	const XMFLOAT3 TRIANGLE_UPPER_RIGHT_POS = { 50.0f,0.0f,100.0f };
	const XMFLOAT3 TRIANGLE_UPPER_LEFT_POS = { -50.0f,0.0f,100.0f };

public:
	/*---- メンバ変数 ----*/

	Object3D enemyData;		//データ
	XMFLOAT3 forwardVec;	//正面ベクトル
	int status;				//Enemyの状態
	XMMATRIX matRot;		//回転行列
	EnemyBullet enemyBullet;//敵の弾
	//当たり判定用
	Ray forwardRay;			//正面側のレイ
	Sphere sphere;			//食らい判定用の球
	Triangle forwardTriangle;//索敵用正面三角
	bool isAlive;			//生存フラグ

	int HP;					//ヒットポイント

	//索敵用
	int searchDelayTimer;	//索敵ディレイタイマー
	int searchTimer;		//索敵する時間
	int rotateStatus;		//索敵時のステータス

	//ターゲティング用
	int targetingTimer;
	XMFLOAT3 prevPlayerPos;	//攻撃時

	//攻撃用
	int attackDelayTimer;	//攻撃後の硬直時間
	bool isAttack;			//攻撃フラグ

public:
	/*---- メンバ関数 ----*/

	//コンストラクタ
	Enemy();

	//初期化処理
	void Init(const Camera& cam);

	//生成処理
	void Generate(const Camera& cam, const XMFLOAT3& generatePos);

	//更新処理
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam);

	//描画処理
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);

	//索敵
	void Searching(const Sphere& playerSphere);

	//ターゲティング
	void Targeting(const XMFLOAT3& playerPos);

	//攻撃
	void Attack();

	//正面ベクトル更新
	void UpdateForwardVec(XMFLOAT3& forwardVec, XMMATRIX& matRot);

	//死亡
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