#pragma once
#include"Enemy.h"
#include"Singleton.h"

class EnemyMgr:public Singleton<EnemyMgr>
{
public:
	//�R���X�g���N�^
	friend Singleton<EnemyMgr>;
	EnemyMgr();
	
	//������
	Enemy enemy;

	//Enemy enemy[100];
	
	//����������
	void Init();

	//�X�V����
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam);

	//�`�揈��
	void Draw(const PipeClass::PipelineSet& pipelineSet);

	//�G�̍U���ƃv���C���[�̔���
	void CheckEnemyAttackToPlayer(int HP);

	XMFLOAT3 GetEnemyPos(int num) { return enemy/*[num]*/.enemyData.position; }

};