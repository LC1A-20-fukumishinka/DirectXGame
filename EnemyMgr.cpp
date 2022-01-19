#include "EnemyMgr.h"

EnemyMgr::EnemyMgr()
{
	savePos = {};
	saveDistance = 10000.0f;
}

void EnemyMgr::Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		if (enemy[i].isAlive)
		{
			enemy[i].Update(playerPos, playerSphere, cam);
		}
		else
		{
			enemy[i].Generate(cam, { playerPos.x + GetRand(-100,100),playerPos.y,playerPos.z + GetRand(-100,100) });
		}
	}
}

void EnemyMgr::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		if (enemy[i].isAlive)
		{
			enemy[i].Draw(pipelineSet);
		}
	}
}

void EnemyMgr::CheckEnemyAttackToPlayer(int HP, const Sphere& playerSphere)
{
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		if (enemy[i].isAttack)
		{
			//正面レイ方向に敵がいたら
			if (Collision::CheckRay2Sphere(enemy[i].forwardRay, playerSphere))
			{
				HP--;
			}
		}
	}
}

XMFLOAT3 EnemyMgr::GetNearEnemyPos(const XMFLOAT3& playerPos)
{
	saveDistance = 10000.0f;
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
		}
	}
	return savePos;
}
