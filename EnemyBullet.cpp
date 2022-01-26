#include"EnemyBullet.h"

EnemyBullet::EnemyBullet()
{
	bulletData = {};
	forwardVec = {};
	isAlive = false;
	bulletSphere = {};
	desTimer = 0;
}

void EnemyBullet::Init(const Camera& cam)
{
	bulletData.Init(cam);
	bulletData.scale = { 10.0f, 10.0f, 10.0f };
	bulletSphere.radius = BULLET_RADIUS;
}

void EnemyBullet::Generate(const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec)
{
	//�������W�Ɛ��ʃx�N�g����ݒ�
	bulletData.position = generatePos;
	this->forwardVec = forwardVec;
	isAlive = true;
}

void EnemyBullet::Update(const Camera& cam)
{
	if (!isAlive)return;

	//������܂ł̃^�C�}�[��i�߂�
	desTimer++;
	if (desTimer >= MAX_DES_TIMER)
	{
		Dead();
	}

	moveVec.x = forwardVec.x * BULLET_SPEED;
	moveVec.y = forwardVec.y * BULLET_SPEED;
	moveVec.z = forwardVec.z * BULLET_SPEED;

	//���W���X�V
	bulletData.position.x += moveVec.x;
	bulletData.position.y += moveVec.y;
	bulletData.position.z += moveVec.z;
}

void EnemyBullet::Dead()
{
	desTimer = 0;
	isAlive = false;
}

void EnemyBullet::Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model)
{
	if (isAlive)
	{
		bulletData.modelDraw(model, pipelineSet);
	}
}