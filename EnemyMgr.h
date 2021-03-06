#pragma once
#include"Enemy.h"
#include"Singleton.h"
#include<vector>

class EnemyMgr :public Singleton<EnemyMgr>
{
public:
	/*---- メンバ定数 ----*/
	static const int MAX_ENEMY_COUNT = 20;

	/*---- メンバ変数 ----*/
	Model enemyModel;		//モデル
	int saveNum;			//近い敵の配列番号を保存するための変数
	bool isHit;				//弾がプレイヤーに当たった時に使うフラグ

	//コンストラクタ
	friend Singleton<EnemyMgr>;
	EnemyMgr();

	//お試し
	//Enemy enemy;

	Enemy enemy[MAX_ENEMY_COUNT];

	//初期化処理
	void Init(const Camera& cam);

	//更新処理
	void Update(Camera& cam, const XMFLOAT3& playerPos, const float& angle, const bool& isStop, const bool& isAttack);

	void UpdateData(const Camera& cam);

	//描画処理
	void Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& bulletModel, int particleGH);

	//敵の攻撃とプレイヤーの判定
	bool CheckEnemyAttackToPlayer(int i);

	//プレイヤーから一番近い敵が死ぬ処理
	void DeadNearEnemy();

	XMFLOAT3 GetEnemyPos(int num) { return enemy[num].enemyData.position; }

	XMFLOAT3 GetNearEnemyPos(const XMFLOAT3& playerPos);

	Object3D* GetObj() { return enemy[0].GetObj(); }

	Model* GetModel() { return &enemyModel; }

	/// <summary>
	/// 敵をまとめて生成する
	/// </summary>
	/// <param name="generatePos">呼び出す敵の配列</param>
	void Generate(std::vector<DirectX::XMFLOAT3>& generatePos, std::vector<DirectX::XMFLOAT3>& forwardVec, const Camera& cam);

	void StopSound();

};

//inline float calAngle(const XMFLOAT3& rhs, const XMFLOAT3& lhs)
//{
//	return Dot3D(lhs, rhs) / (Length3D(lhs) * Length3D(rhs));
//}

////3D座標軸での二点間の距離を求める
//inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
//	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
//}