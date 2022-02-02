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
	scale = 10.0f;
	addScale = 0.25f;
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
	enemyData.scale = { scale, scale, scale };
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

			//スケールの更新
			enemyData.scale = { scale, scale, scale };

			//レイの更新処理
			//座標
			forwardRay.start = XMLoadFloat3(&enemyData.position);
			//方向
			forwardRay.dir = XMLoadFloat3(&forwardVec);

			//球座標の更新
			sphere.center = XMLoadFloat3(&enemyData.position);
		}
	}
	else//ここから索敵する敵の処理
	{
	//時間停止していたら処理は通らない
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

			//スケールの更新
			enemyData.scale = { scale, scale, scale };

			//レイの更新処理
			//座標
			forwardRay.start = XMLoadFloat3(&enemyData.position);
			//方向
			forwardRay.dir = XMLoadFloat3(&forwardVec);

			//球座標の更新
			sphere.center = XMLoadFloat3(&enemyData.position);
		}
		else
		{
			//時間停止していたら追尾SEを鳴らさない
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
	//searchTimer++;

	//if (searchTimer >= MAX_SEARCH_TIMER)
	//{
	//	//rotateStatus = GetRand(0, 3);
	//	rotateStatus = 0;
	//	searchTimer = 0;
	//}

	//switch (rotateStatus)
	//{
	//case ROTATE_STATUS_RIGHT:
	//	
	//	break;
	//case ROTATE_STATUS_LEFT:
	//	enemyData.rotation.y -= ROTATE_SPEED;
	//	break;
	//case ROTATE_STATUS_STOP:
	//	break;
	//default:
	//	enemyData.rotation.y += ROTATE_SPEED;
	//	break;
	//}

	//searchDelayTimer++;

	//索敵時は一定方向に回転し続ける
	enemyData.rotation.y += ROTATE_SPEED;

	scale = 10.0f;

	//当たり判定に使うプレイヤーの当たり判定
	Sphere playerSphere = {};
	playerSphere.center = XMLoadFloat3(&playerPos);
	playerSphere.radius = 16;

	//敵とプレイヤーの距離を計算
	float distance = Distance3D(enemyData.position, playerPos);
	//敵とプレイヤーの半径の合計
	float r = 16 + SEARCH_RADIUS;

	//敵の感知範囲とプレイヤーの半径の合計
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
	//敵からプレイヤーへの方向ベクトル
	XMFLOAT3 buff = XMFLOAT3(enemyData.position.x - playerPos.x, enemyData.position.y - playerPos.y, enemyData.position.z - playerPos.z);
	//正面ベクトルをプレイヤーの方向に向ける
	forwardVec = Normalize3D(buff);

	//敵の向いている方向を回転行列から取得する
	XMFLOAT3 directionVec = {};
	UpdateForwardVec(directionVec, matRot);

	//敵の向いているベクトルとプレイヤーへのベクトルの外積を計算する
	float cross = Cross2D({ forwardVec.x,forwardVec.z }, { directionVec.x,directionVec.z });

	//敵の向いているベクトルとプレイヤーへのベクトルの角度を計算する
	float angle = calAngle(forwardVec, directionVec);
	angle = acosf(angle);
	angle = 180.0f - fabs(XMConvertToDegrees(angle));

	//差がものすごく小さいとき(angleがnanのとき)は回転処理しない
	if (!std::isnan(angle))
	{
		if (cross < 0)
		{
			//角度が回転角度より小さかったら角度分回転させる
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
			//角度が回転角度より小さかったら角度分回転させる
			if (angle < TARGET_ROTATE_SPEED)
			{
				enemyData.rotation.y -= angle;
			}
			else
			{
				enemyData.rotation.y -= TARGET_ROTATE_SPEED;
			}
		}
	}

	//敵とプレイヤーの距離を計算
	float dis = Distance3D(enemyData.position, playerPos);
	//敵の感知範囲とプレイヤーの半径の合計
	float sR = 16 + SENSING_RADIUS;

	//感知範囲外かつ、敵の後ろ側だった場合見失うようにする
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

	//敵からプレイヤーに向けてのレイ
	Ray honraiRay = {};
	honraiRay.start = XMLoadFloat3(&enemyData.position);
	honraiRay.dir = XMLoadFloat3(&(playerPos - enemyData.position));

	//追尾中に壁に隠れられたらステータスを捜索に変える
	if (CheckRay2Walls(honraiRay, WallMgr::Instance()->GetWalls(), playerPos))
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//音止める
		targetSE->Stop();
	}

	//スケールを加算する
	scale += addScale;

	if (scale >= 12.0f || scale <= 8.0f)
	{
		addScale *= -1;
	}

	//タイマーを進める
	targetingTimer++;

	//タイマーが規定値に達したら攻撃をするようにする
	if (targetingTimer >= TARGET_TIMER_END)
	{
		targetingTimer = 0;
		//攻撃開始時のプレイヤー座標を保存
		prevPlayerPos = playerPos;

		isAttack = true;
		//射撃サウンドを再生
		shotSE->Play();

		//追尾サウンドを停止
		targetSE->Stop();

		//ステータスを攻撃にする
		status = STATUS_ATTACK;
	}
}

void Enemy::Attack(const XMFLOAT3 &playerPos)
{
	if (!isEnemyTypeSeach)
	{
		//弾を連射させる
		bulletTimer++;

		if (bulletTimer > MAX_BULLET_TIMER)
		{
			bulletTimer = 0;
			for (int i = 0; i < MAX_BULLET; i++)
			{
				//生成している場合処理を飛ばす
				if (enemyBullet[i].isAlive)continue;
				enemyBullet[i].Generate(enemyData.position, forwardVec, isEnemyTypeSeach);
				shotSE->Play();
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
		//壁1つの三角形ぶん回す
		for (int j = 0; j < walls[0].GetFaces().size(); ++j)
		{
			XMVECTOR v;
			if (Collision::CheckRay2Triangle(ray, walls[i].GetFaces()[j], 0, &v))
			{
				XMFLOAT3 wallPos;
				XMStoreFloat3(&wallPos, v);
				float wDis = Distance3D(enemyData.position, wallPos);
				float pDis = Distance3D(enemyData.position, playerPos);
				if (wDis <= pDis)return true;
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
