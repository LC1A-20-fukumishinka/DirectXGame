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
		//�X�V����
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
		if (!enemy[i].isAlive)continue;
		enemy[i].Draw(pipelineSet, enemyModel.GetModel());
		for (int j = 0; j < 20; ++j)
		{
			if (!enemy[i].enemyBullet[j].isAlive)continue;
			EnemyMgr::Instance()->enemy[i].enemyBullet[j].Draw(pipelineSet, bulletModel);
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

void EnemyMgr::Generate(std::vector<DirectX::XMFLOAT3> &generatePos, std::vector<DirectX::XMFLOAT3>& forwardVec, const Camera &cam)
{
//�n���ꂽ�G�̐����z�񂪃}�l�[�W���[�N���X�̍ő�l���傫�������瑁�����^�[��
	if(generatePos.size()>MAX_ENEMY_COUNT) return;

	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		enemy[i].Init(cam);
	}
	for (int i = 0; i < generatePos.size(); i++)
	{
		enemy[i].Generate(cam, generatePos[i], forwardVec[i]);
	}
}
