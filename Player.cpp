#include "Player.h"

Player::Player()
{
	pos = { 0,0,0 };
	direction = { 0,0,0 };
	hp = MAX_HP;
	stopTimeCount = 0;
	stopTImeDelay = STOP_TIME_DELAY;
	attackCount = 0;
	attackFlag = false;
	stopTimeFlag = false;

	model.CreateModel("player");
	obj.scale = { 10.0f,10.0f,10.0f };
	obj.rotation = { 0, angle + 90.0f, 0 };
}

Player::~Player()
{
}

void Player::Init(const Camera& camera)
{
	pos = { 0,0,0 };
	direction = { 0,0,0 };
	hp = MAX_HP;
	stopTimeCount = 0;
	stopTImeDelay = STOP_TIME_DELAY;
	attackCount = 0;
	angle = 0;
	attackFlag = false;
	stopTimeFlag = false;

	obj.Init(camera);
	obj.rotation = { 0, angle + 90.0f, 0 };
}

void Player::Input(Camera& camera)
{
	/*---L[ìp---*/
	/*Ú®*/
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

	/*---L[ìp---*/
	/*Ôâ~*/

	//Ôâ~O
	if (!stopTimeFlag)
	{
		//CTÇ
		if (stopTImeDelay >= STOP_TIME_DELAY)
		{
			//Ôâ~ót
			if (input->KeyTrigger(DIK_E))
			{
				stopTImeDelay = 0;
				stopTimeFlag = true;
			}
		}
		else { stopTImeDelay++; }
	}

	//Ôâ~
	if (stopTimeFlag)
	{
		//êèt[ÅÔâ~ð
		if (stopTimeCount >= STOP_TIME_COUNT)
		{
			stopTimeCount = 0;
			stopTimeFlag = false;
		}
		else { stopTimeCount++; }
	}

	/*U*/

	//UO
	if (!attackFlag)
	{
		if (input->KeyTrigger(DIK_SPACE) && attackDelay == 0) { attackFlag = true; }
		else if (attackDelay > 0) { attackDelay--; }
	}

	//U
	if (attackFlag)
	{
		//»è
		//GÖÌûüxNg
		XMFLOAT3 vec = { 0,0,0 };
		//vec.x = enemyPos.x - obj.position.x;
		vec.x = obj.position.x - enemyPos.x;
		vec.z = enemyPos.z - obj.position.z;
		XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

		//©@ÌûüxNg
		XMFLOAT3 myVec = { 0,0,0 };
		float rad = angle;
		ConvertToRadian(rad);
		myVec.x = cosf(rad);
		myVec.z = sinf(rad);

		//vZ
		float dot = vec.x * myVec.x + vec.z * myVec.z;
		float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
		float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
		float cosTheta = dot / (absA * absB);
		float theta = acosf(-cosTheta);

		ConvertToDegree(theta);
		//ConvertToDegree(dot);
		float attackAngle = theta;
		//float attackAngle = dot;

		//2_ÔÌ£
		float diff = sqrt(
			(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
			(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
			(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));

		//¼aÌv
		float r = 20 + 20;

		//~~~
		if (attackAngle < ATTACK_ANGLE && diff < r) { isHit = true; }
		else { isHit = false; }

		attackDelay = ATTACK_DELAY;
		attackFlag = false;
	}

	if (input->KeyTrigger(DIK_R)) { isHit = false; }
}

void Player::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	//KÈfÅãp·é
	if (hp > 0) { obj.modelDraw(model.GetModel(), pipelineSet); }
}

void Player::Finalize()
{
}
