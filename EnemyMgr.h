#pragma once
#include"Enemy.h"
#include"Singleton.h"

class EnemyMgr:public Singleton<EnemyMgr>
{
public:
	//コンストラクタ
	friend Singleton<EnemyMgr>;
	EnemyMgr();
	
	//お試し
	Enemy enemy;

	//Enemy enemy[100];
	
	//初期化処理
	void Init();

	//更新処理
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam);

	//描画処理
	void Draw(const PipeClass::PipelineSet& pipelineSet);

	//敵の攻撃とプレイヤーの判定
	void CheckEnemyAttackToPlayer(int HP);

	XMFLOAT3 GetEnemyPos(int num) { return enemy/*[num]*/.enemyData.position; }

};