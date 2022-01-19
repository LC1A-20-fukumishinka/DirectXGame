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
	//生成座標と正面ベクトルを設定
	bulletData.position = generatePos;
	this->forwardVec = forwardVec;
	isAlive = true;
}

void EnemyBullet::Update(const Camera& cam)
{
	if (!isAlive)return;

	//消えるまでのタイマーを進める
	desTimer++;
	if (desTimer >= MAX_DES_TIMER)
	{
		Dead();
	}

	//座標を更新
	bulletData.position.x -= forwardVec.x * BULLET_SPEED;
	bulletData.position.y -= forwardVec.y * BULLET_SPEED;
	bulletData.position.z -= forwardVec.z * BULLET_SPEED;
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