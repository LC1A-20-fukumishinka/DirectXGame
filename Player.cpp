#include "Player.h"
#include "Shake.h"
#include <cmath>
Player::Player()
{
	pos = { 0,0,0 };
	//direction = { 0,0,0 };
	vec3 = { 0,0,0 };
	//contVec3 = { 0,0,0 };
	hp = MAX_HP;
	stopTimeCount = 0;
	stopTImeDelay = STOP_TIME_DELAY;
	attackCount = 0;
	attackDelay = 0;
	drawCount = 0;
	angle = 0;
	easeTimer = 0;
	attackFlag = false;
	stopTimeFlag = false;
	isHit = false;
	isDead = false;
	isDamaged = false;
	spriteDeadFlag = false;
	spriteClearFlag = false;
	isEffect = false;
	isOverTrigger = false;

	model.CreateModel("player");
	obj.scale = { 10.0f,10.0f,10.0f };
	obj.rotation = { 0, angle + 90.0f, 0 };

	GH1 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/DEAD_DEAD.png");
	dead.Init(GH1, XMFLOAT2(0.0f, 0.0f));
	dead.position = { 0,-720,0 };
	dead.size = { 1280,720 };

	GH2 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/CLEAR_CLEAR.png");
	clear.Init(GH1, XMFLOAT2(0.0f, 0.0f));
	clear.position = { 0,-720,0 };
	clear.size = { 1280,720 };
}

Player::~Player()
{
}

void Player::Init(const Camera &camera, const XMFLOAT3 &pos)
{
	this->pos = pos;
	//direction = { 0,0,0 };
	vec3 = { 0,0,0 };
	//contVec3 = { 0,0,0 };
	hp = MAX_HP;
	stopTimeCount = 0;
	stopTImeDelay = STOP_TIME_DELAY;
	attackCount = 0;
	attackDelay = 0;
	drawCount = 0;
	angle = 0;
	easeTimer = 0;
	attackFlag = false;
	stopTimeFlag = false;
	isHit = false;
	isDead = false;
	isDamaged = false;
	spriteDeadFlag = false;
	spriteClearFlag = false;
	isEffect = false;
	isOverTrigger = false;

	obj.Init(camera);
	obj.rotation = { 0, angle + 90.0f, 0 };
	obj.color.w = 1.0f;
}

void Player::Input(const Camera &camera)
{
	/*---コントローラー操作用---*/
	if (input->isPadConnect())
	{
		//コントローラーの方向ベクトル
		XMFLOAT3 vec = { 0,0,0 };
		vec.x = input->LStick().x;
		vec.z = input->LStick().y;
		XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

		//入力がある場合
		if (vec.x != 0 && vec.z != 0)
		{
			//自分の方向ベクトル
			XMFLOAT3 myVec = { 0,0,0 };
			float rotY = angle;
			ConvertToRadian(rotY);
			myVec.x = cosf(-rotY);
			myVec.z = sinf(-rotY);

			//コントローラーのアングル
			float contAngle = -atan2f(vec.z, vec.x);
			ConvertToDegree(contAngle);
			if (contAngle < 0.0f) { contAngle += 360.0f; }

			float vx1 = myVec.x - 0;
			float vz1 = myVec.z - 0;
			float vx2 = vec.x - 0;
			float vz2 = vec.z - 0;

			//近い方を判定
			float cross = vx1 * vz2 - vz1 * vx2;
			if (cross < 0)
			{
				angle += MOVE_ANGLE;

				//nearかつオーバーした場合(0を跨いだ時のオーバー判定が微妙)
				if (fabsf(fabsf(angle) - fabsf(contAngle)) <= MOVE_ANGLE && angle > contAngle) {
					isOverTrigger = true;
				}
			}
			else
			{
				angle -= MOVE_ANGLE;

				//nearかつオーバーした場合(0を跨いだ時のオーバー判定が微妙)
				if (fabsf(fabsf(angle) - fabsf(contAngle)) <= MOVE_ANGLE && angle < contAngle) {
					isOverTrigger = true;
				}
			}

			if (angle >= 360.0f) { angle -= 360.0f; }
			else if (angle < 0.0f) { angle += 360.0f; }

			if (isOverTrigger)
			{
				//差分を計算
				/*if (angle > contAngle) {
					angle -= fabsf(fabsf(angle) - fabsf(contAngle));
				}
				else {
					angle += fabsf(fabsf(angle) - fabsf(contAngle));
				}*/

				//直接書き換え
				angle = contAngle;
				isOverTrigger = false;
			}

			obj.rotation = { 0, angle + 90.0f, 0 };

			myVec.x *= MOVE_SPEED;
			myVec.z *= MOVE_SPEED;
			//contVec3 = myVec;
			vec3 = myVec;
		}
		else {
			//contVec3 = { 0,0,0 };
			vec3 = { 0,0,0 };
		}
	}


	/*---キー操作用---*/
	else
	{
		/*移動*/
		if (input->Key(DIK_D) || input->Key(DIK_A))
		{
			if (input->Key(DIK_D))
			{
				angle += MOVE_ANGLE;
			}
			if (input->Key(DIK_A))
			{
				angle -= MOVE_ANGLE;
			}

			obj.rotation = { 0, angle + 90.0f, 0 };
		}

		if (input->Key(DIK_W) || input->Key(DIK_S))
		{
			float rotY = angle;
			ConvertToRadian(rotY);
			vec3.x = cosf(-rotY);
			vec3.z = sinf(-rotY);

			if (input->Key(DIK_W))
			{
				vec3.x *= MOVE_SPEED;
				vec3.z *= MOVE_SPEED;
			}
			if (input->Key(DIK_S))
			{
				vec3.x *= -MOVE_SPEED;
				vec3.z *= -MOVE_SPEED;
			}
		}
		else { vec3 = { 0.0f,0.0f,0.0f }; }
	}

	//デバッグ用
	if (input->KeyTrigger(DIK_DOWN))
	{
		Damaged();
	}
}

void Player::Update(Camera &camera, const XMFLOAT3 &enemyPos)
{
	if (input->isPadConnect())
	{
		pos.x += vec3.x;
		pos.z += vec3.z;
	}
	else
	{
		pos.x += vec3.x;
		pos.z += vec3.z;
	}
	attackFlag = false;

	Vector3 CameraPos = camera.position;
	Vector3 cameraToPlayerVector = Vector3(pos.x, pos.y, pos.z) - camera.position;

	cameraToPlayerVector *= 0.1f;
	camera.position = CameraPos + cameraToPlayerVector;

	Vector3 nowTargetPos = camera.target;

	obj.position = pos;
	obj.Update(camera);

	/*---キー操作用---*/
	/*時間停止*/

	//時間停止外
	if (!stopTimeFlag)
	{
		//CT管理
		if (stopTImeDelay >= STOP_TIME_DELAY)
		{
			if (input->isPadConnect())
			{
				//時間停止受付
				if (input->ButtonTrigger(XINPUT_GAMEPAD_A))
				{
					stopTImeDelay = 0;
					stopTimeFlag = true;
				}
			}
			else
			{
				if (input->KeyTrigger(DIK_RETURN))
				{
					stopTImeDelay = 0;
					stopTimeFlag = true;
				}
			}
		}
		else { stopTImeDelay++; }
	}

	//時間停止中
	if (stopTimeFlag)
	{
		//一定フレーム数で時間停止解除
		if (stopTimeCount >= STOP_TIME_COUNT)
		{
			stopTimeCount = 0;
			stopTimeFlag = false;
		}
		else { stopTimeCount++; }
	}

	/*攻撃*/

	//攻撃前
	if (!attackFlag)
	{
		isHit = false;
		if (input->isPadConnect())
		{
			if (input->RTrigger() >= 0.3 && attackDelay == 0) {
				attackFlag = true;
			}
			else if (attackDelay > 0) { attackDelay--; }
		}
		else
		{
			if (input->KeyTrigger(DIK_SPACE) && attackDelay == 0) { attackFlag = true; }
			else if (attackDelay > 0) { attackDelay--; }
		}
	}

	//攻撃中
	if (attackFlag)
	{
		//判定
		//敵への方向ベクトル
		XMFLOAT3 vec = { 0,0,0 };
		//vec.x = enemyPos.x - obj.position.x;
		vec.x = obj.position.x - enemyPos.x;
		vec.z = enemyPos.z - obj.position.z;
		XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

		//自機の方向ベクトル
		XMFLOAT3 myVec = { 0,0,0 };
		float rad = angle;
		ConvertToRadian(rad);
		myVec.x = cosf(rad);
		myVec.z = sinf(rad);

		//計算
		float dot = vec.x * myVec.x + vec.z * myVec.z;
		float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
		float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
		float cosTheta = dot / (absA * absB);
		float theta = acosf(-cosTheta);

		ConvertToDegree(theta);
		//ConvertToDegree(dot);
		float attackAngle = theta;
		//float attackAngle = dot;

		//2点間の距離
		float dist = sqrtf(
			(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
			(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
			(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));

		//半径の合計
		float r = 15.0f + 32.0f;

		//円×円
		if (attackAngle < ATTACK_ANGLE && dist < r)
		{
			isHit = true;
		}
		else
		{
			isHit = false;
		}

		attackDelay = ATTACK_DELAY;
	}

	//被ダメージ時シェイク
	if (isDamaged)
	{
		camera.SetShift(Shake::GetShake(1.0f, true, false, true));
		damagedCount++;
		if (damagedCount > INVINCIBLE_COUNT)
		{
			isDamaged = false; damagedCount = 0;
		}
	}

	if (spriteDeadFlag)
	{
		XMFLOAT3 pos = dead.position;
		if (pos.y < 0) { pos.y += 10.0f; }
		else { isEffect = false; }
		dead.position = pos;
		dead.SpriteUpdate();
	}

	if (spriteClearFlag)
	{
		XMFLOAT3 pos = clear.position;
		if (pos.y < 0) { pos.y += 10.0f; }
		else { isEffect = false; }
		clear.position = pos;
		clear.SpriteUpdate();
	}
}

void Player::Draw(const PipeClass::PipelineSet &pipelineSet)
{
	//適当なモデルで代用する
	//if (IsDead()) return;

	if (isDamaged)
	{
		if (drawCount % 3 == 0) { obj.color = { 1.0f,1.0f,1.0f,0.3f }; }
		else { obj.color = { 1.0f,1.0f,1.0f,1.0f }; }

		if (drawCount < INVINCIBLE_COUNT) { drawCount++; }
		else { obj.color = { 1.0f,1.0f,1.0f,1.0f };  drawCount = 0; }
	}

	obj.modelDraw(model.GetModel(), pipelineSet);

	if (spriteDeadFlag) { dead.SpriteDraw(); }
	else if (spriteClearFlag) { clear.SpriteDraw(); }
}

void Player::Finalize()
{
}

void Player::PushBack(const XMFLOAT3 &enemyPos)
{
	//半径
	float r = 13.0f + 14.0f;
	//2点間の距離
	/*float dist = sqrtf(
		(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
		(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
		(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));*/
	float dist = sqrtf(
		(enemyPos.x - pos.x) * (enemyPos.x - pos.x) +
		(enemyPos.y - pos.y) * (enemyPos.y - pos.y) +
		(enemyPos.z - pos.z) * (enemyPos.z - pos.z));
	//重なった場合
	if (dist < r)
	{
		//自機への方向ベクトル
		XMFLOAT3 vec3 = { 0,0,0 };
		vec3.x = obj.position.x - enemyPos.x;
		vec3.z = obj.position.z - enemyPos.z;
		XMStoreFloat3(&vec3, XMVector3Normalize(XMLoadFloat3(&vec3)));

		pos.x = enemyPos.x + vec3.x * r;
		pos.z = enemyPos.z + vec3.z * r;
	}
}

void Player::DeathEffect(Camera &camera)
{
	if (IsDead())
	{
		if (!spriteDeadFlag) isEffect = true;
		if (obj.rotation.x > -90.0f) {
			obj.rotation.x -= 1.0f;
		}
		else if (easeTimer < 1.0f)
		{
			easeTimer += 0.01f;
			camera.eye.y = (650 - 250) * easeOutCubic(easeTimer) + 250;
		}
		else {
			spriteDeadFlag = true;
		}
	}
}

bool Player::SetGoalAndCheak(const XMFLOAT3 &lowerLeft, const XMFLOAT3 &upperRight)
{
	if (lowerLeft.x <= pos.x <= upperRight.x &&
		lowerLeft.z <= pos.z <= upperRight.z)
	{
		return true;
	}
	return false;
}

void Player::ClearEffect(Camera &camera, bool setGoalAndCheak)
{
	if (setGoalAndCheak)
	{
		isEffect = true;
		if (easeTimer < 1.0f)
		{
			easeTimer += 0.01f;
			camera.eye.y = (650 - 250) * easeOutCubic(easeTimer) + 250;
		}
		else { spriteClearFlag = true; }
	}
}

