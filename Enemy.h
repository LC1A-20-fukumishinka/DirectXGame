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

	const int TARGET_TIMER_END = 100;			//プレイヤーを見つけた後攻撃するまでの待機時間
	const int ATTACK_DELAY_TIMER_END = 10;		//攻撃後の硬直時間
	const int SEARCH_DELAY_TIMER_END = 300;		//索敵開始した後のディレイタイマー(debug)
	const int MAX_SEARCH_TIMER = 100;			//索敵時左右切り替え用のタイマー

	const XMFLOAT3 TRIANGLE_UPPER_RIGHT_POS = { 50.0f,0.0f,100.0f };
	const XMFLOAT3 TRIANGLE_UPPER_LEFT_POS = { -50.0f,0.0f,100.0f };

	const float ROTATE_SPEED = 8.0f;;	//回転速度
	const float ENEMY_RADIUS = 32.0f;

	const float TARGET_ROTATE_SPEED = 30.0f;

	static const int MAX_BULLET = 20;
	const int MAX_BULLET_TIMER = 10;			//射撃レート
	const float SEARCH_RADIUS = 150.0f;			//索敵範囲
	const float SENSING_RADIUS = 50.0f;			//索敵時にこれ以上近づいたら感知するよ～範囲

public:
	/*---- メンバ変数 ----*/

	Object3D enemyData;		//データ
	XMFLOAT3 forwardVec;	//正面ベクトル
	int status;				//Enemyの状態
	XMMATRIX matRot;		//回転行列
	EnemyBullet enemyBullet[MAX_BULLET];//敵の弾
	//当たり判定用
	Ray forwardRay;			//正面側のレイ
	Sphere sphere;			//食らい判定用の球
	Triangle forwardTriangle;//索敵用正面三角
	bool isAlive;			//生存フラグ
	int saveNum;
	bool isHit;

	int HP;					//ヒットポイント

	//索敵用
	int searchDelayTimer;	//索敵ディレイタイマー
	int searchTimer;		//索敵する時間
	int rotateStatus;		//索敵時のステータス

	//ターゲティング用
	int targetingTimer;
	XMFLOAT3 prevPlayerPos;	//攻撃時

	XMFLOAT3 AttackVec;
	//攻撃用
	int attackDelayTimer;	//攻撃後の硬直時間
	bool isAttack;			//攻撃フラグ
	int bulletTimer;		//射撃レート

	//敵の種類判別用
	bool isEnemyTypeSeach;

	//サウンド
	Sound* targetSE;
	Sound* shotSE;

public:
	/*---- メンバ関数 ----*/

	//コンストラクタ
	Enemy();

	//デストラクタ
	~Enemy();

	//初期化処理
	void Init(const Camera& cam);

	//生成処理
	void Generate(const Camera& cam, const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec);

	//更新処理
	void Update(const XMFLOAT3& playerPos, const float& angle, const bool& isAttack, const bool& isStop);

	//弾の更新処理
	void BulletUpdate();
	//描画処理
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model);

	//索敵
	void Searching(const XMFLOAT3& playerPos);

	//ターゲティング
	void Targeting(const XMFLOAT3& playerPos);

	//攻撃
	void Attack(const XMFLOAT3& playerPos);

	//正面ベクトル更新
	void UpdateForwardVec(XMFLOAT3& forwardVec, XMMATRIX& matRot);

	//レイと壁の当たり判定
	bool CheckRay2Walls(const Ray& ray, std::vector<Wall>& walls, const XMFLOAT3& playerPos);

	XMFLOAT3 GetNearEnemyBulletPos(const XMFLOAT3& playerPos);

	//死亡
	void Dead();

	//正面ベクトルセット用
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

//3D座標軸での二点間の距離を求める
inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
}

inline float calAngle(const XMFLOAT3& rhs, const XMFLOAT3& lhs)
{
	return Dot3D(lhs, rhs) / (Length3D(lhs) * Length3D(rhs));
}