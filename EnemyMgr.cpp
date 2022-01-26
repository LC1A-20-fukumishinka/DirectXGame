#include "EnemyMgr.h"

EnemyMgr::EnemyMgr()
{
	enemyModel.CreateModel("Enemy");
	saveNum = 0;
}

void EnemyMgr::Init(const Camera& cam)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		enemy[i].Init(cam);
	}
}

void EnemyMgr::Update(const XMFLOAT3& playerPos, const float& angle, const bool& isStop, const bool& isAttack)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		if (enemy[i].isAlive)
		{
			enemy[i].Update(playerPos, angle, isAttack, isStop);
		}
		if (isStop)return;
		for (int j = 0; j < 20; ++j)
		{
			enemy[i].enemyBullet[j].Update();
		}
	}
}

void EnemyMgr::UpdateData(const Camera& cam)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		//enemy[i].enemyData.rotation.y += 1.0f;
		//更新処理
		enemy[i].enemyData.Update(cam);
		for (int j = 0; j < 20; ++j)
		{
			enemy[i].enemyBullet[j].bulletData.Update(cam);
		}
	}
}

void EnemyMgr::Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& bulletModel)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		for (int j = 0; j < 20; ++j)
		{
			if (!enemy[i].enemyBullet[j].isAlive)continue;
			EnemyMgr::Instance()->enemy[i].enemyBullet[j].Draw(pipelineSet, bulletModel);
		}
		if (!enemy[i].isAlive)continue;
		enemy[i].Draw(pipelineSet, enemyModel.GetModel());
	}
}

bool EnemyMgr::CheckEnemyAttackToPlayer(int num,const Sphere& playerSphere)
{
	if (!enemy[num].isAttack)return false;
	//正面レイ方向に敵がいたら
	if (Collision::CheckRay2Sphere(enemy[num].forwardRay, playerSphere))
	{
		return true;
	}

	return false;
}

void EnemyMgr::DeadNearEnemy()
{
	enemy[saveNum].Dead();
}

XMFLOAT3 EnemyMgr::GetNearEnemyPos(const XMFLOAT3& playerPos)
{
	XMFLOAT3 savePos = {};
	float saveDistance = 10000.0f;

	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		//生存してる敵だけ計算する
		if (!enemy[i].isAlive)continue;
		//プレイヤーとの距離を測る
		float distance = Distance3D(playerPos, GetEnemyPos(i));
		//暫定最小より小さかったら
		if (saveDistance >= distance)
		{
			//最少を更新する
			saveDistance = distance;
			savePos = GetEnemyPos(i);
			saveNum = i;
		}
	}
	return savePos;
}

void EnemyMgr::Generate(std::vector<DirectX::XMFLOAT3> &generatePos, std::vector<DirectX::XMFLOAT3>& forwardVec, const Camera &cam)
{
//渡された敵の生成配列がマネージャークラスの最大値より大きかったら早期リターン
	if(generatePos.size()>MAX_ENEMY_COUNT) return;

	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		enemy[i].Init(cam);
	}
	for (int i = 0; i < generatePos.size(); i++)
	{
		enemy[i].Generate(cam, generatePos[i], forwardVec[i]);
		//正面ベクトルの初期値
		XMFLOAT3 honraiForwardVec = { 0,0,1 };
		//正面ベクトルの初期値と指定されたベクトルのなす角度を求める
		float angle = calAngle(forwardVec[i], honraiForwardVec);
		angle = acosf(angle);
		angle = XMConvertToDegrees(angle);
		if (forwardVec[i].x < 0)angle *= -1;
		enemy[i].enemyData.rotation.y = angle;
	}
}
