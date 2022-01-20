#pragma once
#include"Enemy.h"
#include"Singleton.h"
#include<vector>

class EnemyMgr:public Singleton<EnemyMgr>
{
public:
	/*---- メンバ定数 ----*/
	static const int MAX_ENEMY_COUNT = 20;

	/*---- メンバ変数 ----*/
	Model enemyModel;		//モデル
	int saveNum;			//近い敵の配列番号を保存するための変数

	//コンストラクタ
	friend Singleton<EnemyMgr>;
	EnemyMgr();

	//お試し
	//Enemy enemy;

	Enemy enemy[MAX_ENEMY_COUNT];
	
	//初期化処理
	void Init(const Camera& cam);

	//更新処理
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam, const bool& isStop);

	void UpdateData(const Camera& cam);

	//描画処理
	void Draw(const PipeClass::PipelineSet& pipelineSet);

	//敵の攻撃とプレイヤーの判定
	bool CheckEnemyAttackToPlayer(int num, const Sphere& playerSphere);

	//プレイヤーから一番近い敵が死ぬ処理
	void DeadNearEnemy();

	XMFLOAT3 GetEnemyPos(int num) { return enemy[num].enemyData.position; }

	XMFLOAT3 GetNearEnemyPos(const XMFLOAT3& playerPos);

	/// <summary>
	/// 敵をまとめて生成する
	/// </summary>
	/// <param name="generatePos">呼び出す敵の配列</param>
	void Generate(std::vector<DirectX::XMFLOAT3> &generatePos, const Camera &cam);

};

//3D座標軸での二点間の距離を求める
inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
}