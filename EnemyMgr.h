#pragma once
#include"Enemy.h"
#include"Singleton.h"
#include<vector>

class EnemyMgr:public Singleton<EnemyMgr>
{
public:
	/*---- �����o�萔 ----*/
	static const int MAX_ENEMY_COUNT = 20;

	/*---- �����o�ϐ� ----*/
	Model enemyModel;		//���f��
	int saveNum;			//�߂��G�̔z��ԍ���ۑ����邽�߂̕ϐ�

	//�R���X�g���N�^
	friend Singleton<EnemyMgr>;
	EnemyMgr();

	//������
	//Enemy enemy;

	Enemy enemy[MAX_ENEMY_COUNT];
	
	//����������
	void Init(const Camera& cam);

	//�X�V����
	void Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam, const bool& isStop);

	void UpdateData(const Camera& cam);

	//�`�揈��
	void Draw(const PipeClass::PipelineSet& pipelineSet);

	//�G�̍U���ƃv���C���[�̔���
	bool CheckEnemyAttackToPlayer(int num, const Sphere& playerSphere);

	//�v���C���[�����ԋ߂��G�����ʏ���
	void DeadNearEnemy();

	XMFLOAT3 GetEnemyPos(int num) { return enemy[num].enemyData.position; }

	XMFLOAT3 GetNearEnemyPos(const XMFLOAT3& playerPos);

	/// <summary>
	/// �G���܂Ƃ߂Đ�������
	/// </summary>
	/// <param name="generatePos">�Ăяo���G�̔z��</param>
	void Generate(std::vector<DirectX::XMFLOAT3> &generatePos, const Camera &cam);

};

//3D���W���ł̓�_�Ԃ̋��������߂�
inline float Distance3D(const XMFLOAT3& lhs, const XMFLOAT3& rhs) {
	return sqrtf(powf(lhs.x - rhs.x, 2.0f) + powf(lhs.y - rhs.y, 2.0f) + powf(lhs.z - rhs.z, 2.0f));
}