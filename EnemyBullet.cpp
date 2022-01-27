#include"EnemyBullet.h"
#include"WallMgr.h"
EnemyBullet::EnemyBullet()
{
	bulletData = {};
	forwardVec = {};
	isAlive = false;
	bulletSphere = {};
	desTimer = 0;
	status = BULLET_STATUS_ALIVE;
}

void EnemyBullet::Init(const Camera &cam)
{
	bulletData.Init(cam);
	bulletData.scale = { 10.0f, 10.0f, 10.0f };
	bulletSphere.radius = BULLET_RADIUS;
	status = BULLET_STATUS_ALIVE;
	desTimer = 0;
	isAlive = false;
}

void EnemyBullet::Generate(const XMFLOAT3 &generatePos, const XMFLOAT3 &forwardVec)
{
	//生成座標と正面ベクトルを設定
	bulletData.position = generatePos;
	this->forwardVec = forwardVec;
	isAlive = true;
	desTimer = 0;
	bulletData.color = { 1, 1, 1, 1 };
}

void EnemyBullet::Update()
{
	effect.Update();
	if (!isAlive)return;

	switch (status)
	{
	case BULLET_STATUS_ALIVE:

		//消えるまでのタイマーを進める
		desTimer++;
		if (desTimer >= MAX_DES_TIMER)
		{
			Dead();
		}

		moveVec.x = forwardVec.x * BULLET_SPEED;
		moveVec.y = forwardVec.y * BULLET_SPEED;
		moveVec.z = forwardVec.z * BULLET_SPEED;

		if (WallMgr::Instance()->CheckWallBullet(bulletData.position, moveVec))
		{
			isAlive = false;
		}

		//座標を更新
		bulletData.position.x += moveVec.x;
		bulletData.position.y += moveVec.y;
		bulletData.position.z += moveVec.z;

		break;
	case BULLET_STATUS_EXPLOSION:
		Explosion();
		bulletData.color = { (49 / 255.0f),(78 / 255.0f),(97 / 255.0f),(255 / 255.0f) };
		break;
	default:
		break;
	}
}

void EnemyBullet::Explosion()
{
	if (explosionTimer == 0)
	{
		effect.Bomb(bulletData.position, static_cast<float>(EXPLOSION_RADIUS) / MAX_EXPLOSION_TIMER * 2, MAX_EXPLOSION_TIMER);
	}
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

void EnemyBullet::Draw(const PipeClass::PipelineSet &pipelineSet, const ModelObject &model)
{
	if (isAlive)
	{
		bulletData.modelDraw(model, pipelineSet);
	}
	effect.BufferUpdate();

	effect.Draw();
}