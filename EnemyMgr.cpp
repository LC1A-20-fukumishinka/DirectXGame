#include "EnemyMgr.h"

EnemyMgr::EnemyMgr()
{
}

void EnemyMgr::Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam)
{
	if (enemy.isAlive)
	{
		enemy.Update(playerPos, playerSphere, cam);
	}
	else
	{
		enemy.Generate(cam, { 0,0,0 });
	}
}

void EnemyMgr::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	if (enemy.isAlive)
	{
		enemy.Draw(pipelineSet);
	}
}

void EnemyMgr::CheckEnemyAttackToPlayer(int HP)
{
	if (enemy.isAttack)
	{
		
	}
}
