#pragma once
#include"Enemy.h"
#include"Singleton.h"

class EnemyMgr:public Singleton<EnemyMgr>
{
public:
	/*---- �����o�萔 ----*/
	static const int MAX_ENEMY_COUNT = 5;

	/*---- �����o�ϐ� ----*/
	XMFLOAT3 savePos;
	float saveDistance;

	//�R���X�g���N�^
	friend Singleton<EnemyMgr>;
	EnemyMgr();
	
	//������
	//Enemy enemy;

	Enemy enemy[MAX_ENEMY_COUNT];
	
	//����������
	void Init();

	//�X�V����
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam);

	//�`�揈��
	void Draw(const PipeClass::PipelineSet& pipelineSet);

	//�G�̍U���ƃv���C���[�̔���
	void CheckEnemyAttackToPlayer(int HP, const Sphere& playerSphere);

	XMFLOAT3 GetEnemyPos(int num) { return enemy[num].enemyData.position; }

	XMFLOAT3 GetNearEnemyPos(const XMFLOAT3& playerPos);

};

//3D���W���ł̓�_�Ԃ̋��������߂�
inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
}