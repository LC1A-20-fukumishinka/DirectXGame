#include"Enemy.h"

Enemy::Enemy()
{
	status = STATUS_SEARCH;
	forwardVec = { 0,0,1 };
	forwardRay = {};
	targetingTimer = 0;
	prevPlayerPos = {};
	isAlive = false;
	HP = 0;
	attackDelayTimer = 0;
	isAttack = false;
	searchDelayTimer = 0;
	searchTimer = 0;
	rotateStatus = 0;
}

void Enemy::Init(const Camera& cam)
{
	enemyModel.CreateModel("box");
	status = STATUS_SEARCH;
	forwardVec = { 0,0,1 };
	enemyData.scale = { 10.0f, 10.0f, 10.0f };
	enemyData.position = { 0,10,0 };
	enemyData.Init(cam);
	enemyData.type = Object3D::Box;
	forwardRay = {};
	targetingTimer = 0;
	prevPlayerPos = {};
	isAlive = false;
	searchDelayTimer = SEARCH_DELAY_TIMER_END;
	HP = 100;
}

void Enemy::Generate(const Camera& cam, const XMFLOAT3& generatePos)
{
	Init(cam);
	isAlive = true;
	this->enemyData.position = generatePos;
}

void Enemy::Update(const XMFLOAT3& playerPos, const Sphere& playerSphere, const Camera& cam)
{
	//生成済みじゃなかったら生成する
	if (!isAlive)return;

	//更新処理
	enemyData.Update(cam);

	//ステータスによって処理を分ける
	switch (status)
	{
	case STATUS_SEARCH:
		Searching(playerSphere);
		break;
	case STATUS_TARGET:
		Targeting(playerPos);
		break;
	case STATUS_ATTACK:
		Attack();
		break;
	default:
		break;
	}

	//正面ベクトルの更新
	XMMATRIX matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(enemyData.rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(enemyData.rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(enemyData.rotation.y));
	UpdateForwardVec(forwardVec, matRot);

	//レイの更新処理
	//座標
	forwardRay.start = XMLoadFloat3(&enemyData.position);
	//方向
	forwardRay.dir = XMLoadFloat3(&forwardVec);

	//球座標の更新
	sphere.center = XMLoadFloat3(&enemyData.position);

	//三角形座標の更新
	forwardTriangle.p0 = XMLoadFloat3(&enemyData.position);
	
	//右上
	XMFLOAT3 p1 = enemyData.position;
	p1 = AddXMFLOAT3(p1, TRIANGLE_UPPER_RIGHT_POS);
	p1 = MulXMFLOAT3(p1, forwardVec);
	p1.y = 10;
	forwardTriangle.p1 = XMLoadFloat3(&p1);

	//左上
	XMFLOAT3 p2 = enemyData.position;
	p2 = AddXMFLOAT3(p2, TRIANGLE_UPPER_LEFT_POS);
	p2 = MulXMFLOAT3(p2, forwardVec);
	p2.y = 10;
	forwardTriangle.p2 = XMLoadFloat3(&p2);
}

void Enemy::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	if (isAlive)
	{
		enemyData.modelDraw(enemyModel.GetModel(), pipelineSet);
	}
}

void Enemy::Searching(const Sphere& playerSphere)
{
	searchTimer++;

	if (searchTimer >= MAX_SEARCH_TIMER)
	{
		rotateStatus = GetRand(0, 4);
		searchTimer = 0;
	}

	switch (rotateStatus)
	{
	case ROTATE_STATUS_RIGHT:
		enemyData.rotation.y += XM_PI / 9.0f;
		break;
	case ROTATE_STATUS_LEFT:
		enemyData.rotation.y -= XM_PI / 9.0f;
		break;
	case ROTATE_STATUS_STOP:
		break;
	default:
		enemyData.rotation.y += XM_PI / 9.0f;
		break;
	}

	//searchDelayTimer++;

	////正面レイ方向に敵がいたら
	//if (Collision::CheckRay2Sphere(forwardRay, playerSphere) && searchDelayTimer >= SEARCH_DELAY_TIMER_END)
	//{
	//	//ステータスをターゲティングにする
	//	status = STATUS_TARGET;
	//	searchTimer = 30;
	//	searchDelayTimer = 0;
	//}

	
	if (Collision::CheckSphere2Triangle(playerSphere, forwardTriangle))
	{
		//ステータスをターゲティングにする
		status = STATUS_TARGET;
		searchTimer = 30;
		searchDelayTimer = 0;
	}
}

void Enemy::Targeting(const XMFLOAT3& playerPos)
{
	XMFLOAT3 buff = XMFLOAT3(enemyData.position.x - playerPos.x, enemyData.position.y - playerPos.y, enemyData.position.z - playerPos.z);
	//正面ベクトルをプレイヤーの方向に向ける
	forwardVec = Normalize3D(buff);

	//タイマーを進める
	targetingTimer++;

	if (targetingTimer >= TARGET_TIMER_END)
	{
		targetingTimer = 0;
		//攻撃開始時のプレイヤー座標を保存
		prevPlayerPos = playerPos;

		isAttack = true;

		//ステータスを攻撃にする
		status = STATUS_ATTACK;
	}
}

void Enemy::Attack()
{
	isAttack = false;
	
	attackDelayTimer++;

	if (attackDelayTimer >= ATTACK_DELAY_TIMER_END)
	{
		attackDelayTimer = 0;
		status = STATUS_SEARCH;
	}
}

void Enemy::UpdateForwardVec(XMFLOAT3& forwardVec, XMMATRIX& matRot)
{
	//正面ベクトルのフォーマット
	XMFLOAT3 honraiForwardVec = { 0,0,1 };
	XMVECTOR honraiForwardVector = XMLoadFloat3(&honraiForwardVec);

	//正面ベクトルのフォーマットに自機の回転行列をかける
	honraiForwardVector = XMVector3Transform(honraiForwardVector, matRot);

	//かけた正面ベクトルを正規化する
	honraiForwardVector = XMVector3Normalize(honraiForwardVector);

	//正面ベクトルを更新する
	XMStoreFloat3(&forwardVec, honraiForwardVector);
}
