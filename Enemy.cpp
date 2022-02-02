#include"Enemy.h"
#include "DirectInput.h"
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
	bulletTimer = 0;
	matRot = XMMatrixIdentity();
	saveNum = 0;
	isHit = false;
	isEnemyTypeSeach = true;
	int targetData = Sound::SoundLoadWave("Resources/sounds/SE_Find.wav");
	targetSE = new Sound(targetData);
	int shotData = Sound::SoundLoadWave("Resources/sounds/SE_Shot.wav");
	shotSE = new Sound(shotData);
}


Enemy::~Enemy()
{
	delete targetSE;
	delete shotSE;
}

void Enemy::Init(const Camera &cam)
{
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
	isAttack = false;
	searchDelayTimer = SEARCH_DELAY_TIMER_END;
	HP = 100;
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		enemyBullet[i].Init(cam);
	}
}

void Enemy::Generate(const Camera &cam, const XMFLOAT3 &generatePos, const XMFLOAT3 &forwardVec)
{
	//Init(cam);
	isAlive = true;
	this->enemyData.position = generatePos;
	this->forwardVec = forwardVec;
}

void Enemy::Update(const XMFLOAT3 &playerPos, const float &angle, const bool &isAttack, const bool &isStop)
{
	//生成済みじゃなかったら生成する
	if (!isAlive)return;

	//敵のステータスによって処理を分ける
	if (!isEnemyTypeSeach)
	{
		if (isStop)
		{
			XMFLOAT3 nearBulletPos = GetNearEnemyBulletPos(playerPos);
			if (isAttack)
			{
				if (enemyBullet[saveNum].isAlive)
				{
					//判定
					//敵への方向ベクトル
					XMFLOAT3 vec = { 0,0,0 };
					//vec.x = enemyPos.x - obj.position.x;
					vec.x = playerPos.x - nearBulletPos.x;
					vec.z = nearBulletPos.z - playerPos.z;
					XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

					//自機の方向ベクトル
					XMFLOAT3 myVec = { 0,0,0 };
					float rad = angle;
					rad = XMConvertToRadians(rad);
					myVec.x = cosf(rad);
					myVec.z = sinf(rad);

					//計算
					float dot = vec.x * myVec.x + vec.z * myVec.z;
					float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
					float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
					float cosTheta = dot / (absA * absB);
					float theta = acosf(-cosTheta);

					theta = XMConvertToDegrees(theta);
					//ConvertToDegree(dot);
					float attackAngle = theta;
					//float attackAngle = dot;

					//2点間の距離
					float distance = Distance3D(playerPos, nearBulletPos);

					//半径の合計
					float r = 30.0f + enemyBullet->BULLET_RADIUS;

					//円×円
					if (attackAngle < 45.0f && distance < r)
					{
						enemyBullet[saveNum].status = enemyBullet->BULLET_STATUS_EXPLOSION;
						enemyBullet[saveNum].bulletData.color = { 49,78,97,255 };
					}
				}
			}
		}
		else
		{
			isHit = false;
			//弾の処理
			for (int i = 0; i < MAX_BULLET; ++i)
			{
				if (!enemyBullet[i].isAlive)continue;
				if (enemyBullet[i].status != enemyBullet->BULLET_STATUS_EXPLOSION)continue;
				XMFLOAT3 enemyPos1 = enemyBullet[i].bulletData.position;
				for (int j = 0; j < MAX_BULLET; ++j)
				{
					if (!enemyBullet[j].isAlive)continue;
					if (enemyBullet[j].status != enemyBullet->BULLET_STATUS_ALIVE)continue;
					if (i == j)continue;
					XMFLOAT3 enemyPos2 = enemyBullet[j].bulletData.position;
					//2点間の距離を求める
					float distance = Distance3D(enemyPos1, enemyPos2);
					//半径の和を求める
					float r = enemyBullet[i].EXPLOSION_RADIUS + enemyBullet[j].BULLET_RADIUS;
					if (distance < r)
					{
						enemyBullet[j].status = enemyBullet->BULLET_STATUS_EXPLOSION;
					}
				}
				//敵との距離を求める
				float distance = Distance3D(enemyPos1, enemyData.position);
				float r = enemyBullet[i].EXPLOSION_RADIUS + ENEMY_RADIUS;
				if (distance < r)
				{
					Dead();
				}
			}

			//弾の処理
			for (int i = 0; i < MAX_BULLET; ++i)
			{
				if (!enemyBullet[i].isAlive)continue;
				if (enemyBullet[i].status != enemyBullet->BULLET_STATUS_ALIVE)continue;

				XMFLOAT3 enemyPos1 = enemyBullet[i].bulletData.position;

				//プレイヤーとの当たり判定
				float pDistance = Distance3D(enemyPos1, playerPos);
				float playerRadius = 8.0f;
				float pR = enemyBullet->BULLET_RADIUS + playerRadius;
				if (pDistance < pR)
				{
					isHit = true;
				}
			}

			status = STATUS_ATTACK;

			//ステータスによって処理を分ける
			switch (status)
			{
			case STATUS_SEARCH:
				//Searching(playerSphere);
				break;
			case STATUS_TARGET:
				//Targeting(playerPos);
				break;
			case STATUS_ATTACK:
				Attack(playerPos);
				break;
			default:
				break;
			}

			//正面ベクトルの更新
			matRot = XMMatrixIdentity();
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
	}
	else//ここから
	{
		if (!isStop)
		{
			isHit = false;
			//ステータスによって処理を分ける
			switch (status)
			{
			case STATUS_SEARCH:
				Searching(playerPos);
				break;
			case STATUS_TARGET:
				Targeting(playerPos);
				break;
			case STATUS_ATTACK:
				Attack(playerPos);
				break;
			default:
				break;
			}

			//正面ベクトルの更新
			matRot = XMMatrixIdentity();
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
		else
		{
			targetSE->Stop();
		}
	}
}


void Enemy::BulletUpdate()
{
	//弾の更新
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		enemyBullet[i].Update();
	}
}

void Enemy::Draw(const PipeClass::PipelineSet &pipelineSet, const ModelObject &model)
{
	if (isAlive)
	{
		enemyData.modelDraw(model, pipelineSet);
	}
}

void Enemy::Searching(const XMFLOAT3 &playerPos)
{
	searchTimer++;

	if (searchTimer >= MAX_SEARCH_TIMER)
	{
		//rotateStatus = GetRand(0, 3);
		rotateStatus = 0;
		searchTimer = 0;
	}

	switch (rotateStatus)
	{
	case ROTATE_STATUS_RIGHT:
		enemyData.rotation.y += ROTATE_SPEED;
		break;
	case ROTATE_STATUS_LEFT:
		enemyData.rotation.y -= ROTATE_SPEED;
		break;
	case ROTATE_STATUS_STOP:
		break;
	default:
		enemyData.rotation.y += ROTATE_SPEED;
		break;
	}

	//searchDelayTimer++;


	Sphere playerSphere = {};
	playerSphere.center = XMLoadFloat3(&playerPos);
	playerSphere.radius = 16;

	//敵とプレイヤーの距離を計算
	float distance = Distance3D(enemyData.position, playerPos);
	float r = 16 + SEARCH_RADIUS;

	float sR = 16 + SENSING_RADIUS;

	//ある一定の距離以上近くにプレイヤーがいたら
	if (distance < sR)
	{
		//ステータスをターゲティングにする
		status = STATUS_TARGET;
		targetSE->PlayLoop();
		searchTimer = 30;
		searchDelayTimer = 0;
	}

	//正面レイ方向に壁がなく、敵がいたら
	if (Collision::CheckRay2Sphere(forwardRay, playerSphere) && !CheckRay2Walls(forwardRay, WallMgr::Instance()->GetWalls(), playerPos) && distance < r)
	{
		//ステータスをターゲティングにする
		status = STATUS_TARGET;
		targetSE->PlayLoop();
		searchTimer = 30;
		searchDelayTimer = 0;
	}


	//if (Collision::CheckSphere2Triangle(playerSphere, forwardTriangle))
	//{
	//	//ステータスをターゲティングにする
	//	status = STATUS_TARGET;
	//	searchTimer = 30;
	//	searchDelayTimer = 0;
	//}
}

void Enemy::Targeting(const XMFLOAT3 &playerPos)
{
	XMFLOAT3 buff = XMFLOAT3(enemyData.position.x - playerPos.x, enemyData.position.y - playerPos.y, enemyData.position.z - playerPos.z);
	//正面ベクトルをプレイヤーの方向に向ける
	forwardVec = Normalize3D(buff);

	//向いている方向
	XMFLOAT3 distance = {};
	UpdateForwardVec(distance, matRot);

	float cross = Cross2D({ forwardVec.x,forwardVec.z }, { distance.x,distance.z });

	float angle = calAngle(forwardVec, distance);

	angle = acosf(angle);

	angle = 180.0f - fabs(XMConvertToDegrees(angle));

	if (cross < 0)
	{
		if (angle < TARGET_ROTATE_SPEED)
		{
			enemyData.rotation.y += angle;
		}
		else
		{
			enemyData.rotation.y += TARGET_ROTATE_SPEED;
		}
	}
	else
	{
		if (angle < TARGET_ROTATE_SPEED)
		{
			enemyData.rotation.y -= angle;
		}
		else
		{
			enemyData.rotation.y -= TARGET_ROTATE_SPEED;
		}
	}

	//敵とプレイヤーの距離を計算
	float dis = Distance3D(enemyData.position, playerPos);
	float sR = 16 + SENSING_RADIUS;

	if (cross > 0.75 && cross < 1 && dis > sR)
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//音止める
		targetSE->Stop();
	}
	else if (cross < -0.75 && cross > -1 && dis > sR)
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//音止める
		targetSE->Stop();
	}

	//追尾中に壁に隠れられたらステータスを捜索に変える
	if (CheckRay2Walls(forwardRay, WallMgr::Instance()->GetWalls(), playerPos))
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//音止める
		targetSE->Stop();
	}

	//音鳴らす

	//タイマーを進める
	targetingTimer++;

	if (targetingTimer >= TARGET_TIMER_END)
	{
		targetingTimer = 0;
		//攻撃開始時のプレイヤー座標を保存
		prevPlayerPos = playerPos;

		isAttack = true;
		shotSE->Play();
		XMFLOAT3 honraiForwardVec = { -forwardVec.x,-forwardVec.y,-forwardVec.z };

		//enemyBullet[0].Generate(enemyData.position, distance, isEnemyTypeSeach);

		//音止める
		targetSE->Stop();

		//ステータスを攻撃にする
		status = STATUS_ATTACK;
	}
}

void Enemy::Attack(const XMFLOAT3 &playerPos)
{
	if (!isEnemyTypeSeach)
	{
		bulletTimer++;

		if (bulletTimer > MAX_BULLET_TIMER)
		{
			bulletTimer = 0;
			for (int i = 0; i < MAX_BULLET; i++)
			{
				if (enemyBullet[i].isAlive)continue;
				enemyBullet[i].Generate(enemyData.position, forwardVec, isEnemyTypeSeach);
				break;
			}
		}
	}
	else
	{
		isAttack = false;


		attackDelayTimer++;

		if (attackDelayTimer >= ATTACK_DELAY_TIMER_END)
		{
			enemyBullet[0].Generate(enemyData.position, forwardVec, isEnemyTypeSeach);
			shotSE->Play();

			attackDelayTimer = 0;
			Sphere playerSphere = {};
			playerSphere.center = XMLoadFloat3(&playerPos);
			playerSphere.radius = 16;
			if (Collision::CheckRay2Sphere(forwardRay, playerSphere) && !CheckRay2Walls(forwardRay, WallMgr::Instance()->GetWalls(), playerPos))
			{
				isHit = true;
			}
			status = STATUS_SEARCH;
		}
	}
}

void Enemy::UpdateForwardVec(XMFLOAT3 &forwardVec, XMMATRIX &matRot)
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

bool Enemy::CheckRay2Walls(const Ray &ray, std::vector<Wall> &walls, const XMFLOAT3 &playerPos)
{
	//正面レイ方向に壁があったら
	//壁一つ一つ回す
	for (int i = 0; i < walls.size(); ++i)
	{
		XMFLOAT3 wallPos = walls[i].GetPos();
		if (Distance3D(enemyData.position, wallPos) > Distance3D(enemyData.position, playerPos)) continue;

		//壁1つの三角形ぶん回す
		for (int j = 0; j < walls[0].GetFaces().size(); ++j)
		{
			if (Collision::CheckRay2Triangle(forwardRay, walls[i].GetFaces()[j]))
			{
				return true;
			}
		}
	}
	return false;
}

XMFLOAT3 Enemy::GetNearEnemyBulletPos(const XMFLOAT3 &playerPos)
{
	XMFLOAT3 savePos = {};
	float saveDistance = 10000.0f;

	for (int i = 0; i < MAX_BULLET; ++i)
	{
		//生存してる敵だけ計算する
		if (!enemyBullet[i].isAlive)continue;
		//プレイヤーとの距離を測る
		float distance = Distance3D(playerPos, enemyBullet[i].bulletData.position);
		//暫定最小より小さかったら
		if (saveDistance >= distance)
		{
			//最少を更新する
			saveDistance = distance;
			savePos = enemyBullet[i].bulletData.position;
			saveNum = i;
		}
	}
	return savePos;
}

void Enemy::Dead()
{
	targetSE->Stop();
	isAlive = false;
	//for (int i = 0; i < MAX_BULLET; i++)
	//{
	//	enemyBullet[i].Dead();
	//}
}
