#include"EnemyBullet.h"
#include"WallMgr.h"
EnemyBullet::EnemyBullet()
{
	bulletData = {};
	forwardVec = {};
	isAlive = false;
	bulletSphere = {};
	desTimer = 0;
	speed = 0;
	status = BULLET_STATUS_ALIVE;
}

void EnemyBullet::Init(const Camera &cam)
{
	bulletData.Init(cam);
	bulletData.scale = { 10.0f, 10.0f, 10.0f };
	bulletSphere.radius = BULLET_RADIUS;
	status = BULLET_STATUS_ALIVE;
	desTimer = 0;
	speed = 0;
	isAlive = false;
}

void EnemyBullet::Generate(const XMFLOAT3 &generatePos, const XMFLOAT3 &forwardVec, const bool& isEnemyTypeSearch)
{
	//生成座標と正面ベクトルを設定
	XMFLOAT3 honraiGeneratePos = AddXMFLOAT3(generatePos, forwardVec * GENERATE_POS);
	bulletData.position = honraiGeneratePos;
	this->forwardVec = forwardVec;
	isAlive = true;
	desTimer = 0;
	bulletData.color = { 1, 1, 1, 1 };
	bulletData.scale.x = 40.0f;
	bulletData.scale.y = 40.0f;
	bulletData.scale.z = 1200.0f;
	if (isEnemyTypeSearch)speed = BULLET_SPEED_FAST;
	else speed = BULLET_SPEED;
	XMFLOAT3 honraiForwardVec = { 0,0,1 };
	//正面ベクトルの初期値と指定されたベクトルのなす角度を求める
	float angle = calAngle(forwardVec, honraiForwardVec);
	angle = acosf(angle);
	angle = XMConvertToDegrees(angle);
	if (forwardVec.x < 0)angle *= -1;
	bulletData.rotation.y = angle;
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

		moveVec.x = forwardVec.x * speed;
		moveVec.y = forwardVec.y * speed;
		moveVec.z = forwardVec.z * speed;

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