#pragma once
#include"Enemy.h"
#include"Singleton.h"

class EnemyMgr:public Singleton<EnemyMgr>
{
public:
	/*---- メンバ定数 ----*/
	static const int MAX_ENEMY_COUNT = 5;

	/*---- メンバ変数 ----*/
	XMFLOAT3 savePos;
	float saveDistance;

	//コンストラクタ
	friend Singleton<EnemyMgr>;
	EnemyMgr();
	
	//お試し
	//Enemy enemy;

	Enemy enemy[MAX_ENEMY_COUNT];
	
	//初期化処理
	void Init();

	//更新処理
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam);

	//描画処理
	void Draw(const PipeClass::PipelineSet& pipelineSet);

	//敵の攻撃とプレイヤーの判定
	void CheckEnemyAttackToPlayer(int HP, const Sphere& playerSphere);

	XMFLOAT3 GetEnemyPos(int num) { return enemy[num].enemyData.position; }

	XMFLOAT3 GetNearEnemyPos(const XMFLOAT3& playerPos);

};

//3D座標軸での二点間の距離を求める
inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
}