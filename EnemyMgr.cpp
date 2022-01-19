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
			enemy[i].Draw(pipelineSet, enemyModel.GetModel());
		}
	}
}

bool EnemyMgr::CheckEnemyAttackToPlayer(int num,const Sphere& playerSphere)
{
	if (!enemy[num].isAttack)return false;
	//���ʃ��C�����ɓG��������
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
		//�������Ă�G�����v�Z����
		if (!enemy[i].isAlive)continue;
		//�v���C���[�Ƃ̋����𑪂�
		float distance = Distance3D(playerPos, GetEnemyPos(i));
		//�b��ŏ���菬����������
		if (saveDistance >= distance)
		{
			//�ŏ����X�V����
			saveDistance = distance;
			savePos = GetEnemyPos(i);
			saveNum = i;
		}
	}
	return savePos;
}
