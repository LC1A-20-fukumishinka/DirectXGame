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

void EnemyMgr::Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam, const bool& isStop)
{
	if (isStop)return;

	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		if (enemy[i].isAlive)
		{
			enemy[i].Update(playerPos, playerSphere, cam);
		}
		else
		{
			//enemy[i].Generate(cam, { playerPos.x + GetRand(-100,100),playerPos.y,playerPos.z + GetRand(-100,100) });
		}
	}
}

void EnemyMgr::UpdateData(const Camera& cam)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		//更新処理
		enemy[i].enemyData.Update(cam);
		enemy[i].enemyBullet.bulletData.Update(cam);
	}
}

void EnemyMgr::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		if (enemy[i].isAlive)
		{
			enemy[i].Draw(pipelineSet, enemyModel.GetModel());
		}
	}
}

bool EnemyMgr::CheckEnemyAttackToPlayer(int num, const Sphere& playerSphere, std::vector<Wall>& walls)
{
	if (!enemy[num].isAttack)return false;
	//正面レイ方向に壁があったら
	for (int i = 0; i < walls.size(); i++)
	{
		XMFLOAT3 pos = { walls[i].wallObj.position.x,enemy[num].enemyData.position.y,walls[i].wallObj.position.z };
		walls[i].sphere.center = XMLoadFloat3(&pos);
		if (Collision::CheckRay2Sphere(enemy[num].forwardRay, walls[i].sphere))
		{
			enemy[num].enemyBullet.Dead();
			return false;
		}
	}

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

void EnemyMgr::Generate(std::vector<DirectX::XMFLOAT3> &generatePos, const Camera &cam)
{
//渡された敵の生成配列がマネージャークラスの最大値より大きかったら早期リターン
	if(generatePos.size()>MAX_ENEMY_COUNT) return;

	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		enemy[i].Init(cam);
	}
	for (int i = 0; i < generatePos.size(); i++)
	{
		enemy[i].Generate(cam, generatePos[i]);
	}
}
