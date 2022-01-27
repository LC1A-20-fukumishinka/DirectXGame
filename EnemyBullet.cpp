#include"EnemyBullet.h"

EnemyBullet::EnemyBullet()
{
	bulletData = {};
	forwardVec = {};
	isAlive = false;
	bulletSphere = {};
	desTimer = 0;
	status = BULLET_STATUS_ALIVE;
}

void EnemyBullet::Init(const Camera& cam)
{
	bulletData.Init(cam);
	bulletData.scale = { 10.0f, 10.0f, 10.0f };
	bulletSphere.radius = BULLET_RADIUS;
	status = BULLET_STATUS_ALIVE;
	desTimer = 0;
	isAlive = false;
}

void EnemyBullet::Generate(const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec)
{
	//�������W�Ɛ��ʃx�N�g����ݒ�
	bulletData.position = generatePos;
	this->forwardVec = forwardVec;
	isAlive = true;
}

void EnemyBullet::Update()
{
	if (!isAlive)return;

	switch (status)
	{
	case BULLET_STATUS_ALIVE:

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

		break;
	case BULLET_STATUS_EXPLOSION:
		Explosion();
		bulletData.color = { 49,78,97,255 };
		break;
	default:
		break;
	}
}

void EnemyBullet::Explosion()
{
	explosionTimer++;
	if (explosionTimer > MAX_EXPLOSION_TIMER)
	{
		explosionTimer = 0;
		Dead();
	}
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