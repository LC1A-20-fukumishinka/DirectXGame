#include "Player.h"

Player::Player()
{
	pos = { 0,0,0 };
	direction = { 0,0,0 };
	vec3 = { 0,0,0 };
	hp = MAX_HP;
	stopTimeCount = 0;
	stopTImeDelay = STOP_TIME_DELAY;
	attackCount = 0;
	attackDelay = 0;
	drawCount = 0;
	angle = 0;
	attackFlag = false;
	stopTimeFlag = false;
	isHit = false;
	isDead = false;
	isDamaged = false;

	model.CreateModel("player");
	obj.scale = { 10.0f,10.0f,10.0f };
	obj.rotation = { 0, angle + 90.0f, 0 };
}

Player::~Player()
{
}

void Player::Init(const Camera& camera)
{
	pos = {-450, 0, 0};
	direction = { 0,0,0 };
	vec3 = { 0,0,0 };
	hp = MAX_HP;
	stopTimeCount = 0;
	stopTImeDelay = STOP_TIME_DELAY;
	attackCount = 0;
	attackDelay = 0;
	drawCount = 0;
	angle = 0;
	attackFlag = false;
	stopTimeFlag = false;
	isHit = false;
	isDead = false;
	isDamaged = false;

	obj.Init(camera);
	obj.rotation = { 0, angle + 90.0f, 0 };
	obj.color.w = 1.0f;
}

void Player::Input(Camera& camera)
{
	/*---キー操作用---*/
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

void Player::Update(Camera& camera, const XMFLOAT3& enemyPos)
{
	pos.x += vec3.x;
	pos.z += vec3.z;
	camera.eye.x += vec3.x;
	camera.eye.z += vec3.z;
	camera.target = pos;
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
			//時間停止受付
			if (input->KeyTrigger(DIK_E))
			{
				stopTImeDelay = 0;
				stopTimeFlag = true;
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
		if (input->KeyTrigger(DIK_SPACE) && attackDelay == 0) { attackFlag = true; }
		else if (attackDelay > 0) { attackDelay--; }
		isHit = false;
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
		float diff = sqrtf(
			(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
			(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
			(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));

		//半径の合計
		float r = 20 + 20;

		//円×円
		if (attackAngle < ATTACK_ANGLE && diff < r)
		{
			isHit = true;
		}
		else
		{
			isHit = false;
		}

		attackDelay = ATTACK_DELAY;
		attackFlag = false;
	}
}

void Player::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	//適当なモデルで代用する
	if (IsDead()) return;

	if (isDamaged)
	{
		if (drawCount % 3 == 0) { obj.color = { 1.0f,1.0f,1.0f,0.3f }; }
		else { obj.color = { 1.0f,1.0f,1.0f,1.0f }; }

		if (drawCount < 30) { drawCount++; }
		else { obj.color = { 1.0f,1.0f,1.0f,1.0f }; isDamaged = false; drawCount = 0; }
	}

	obj.modelDraw(model.GetModel(), pipelineSet);
}

void Player::Finalize()
{
}
