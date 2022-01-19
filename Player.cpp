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

	model.CreateModel("chr_sword");
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

void Player::Update(Camera& camera, const XMFLOAT3& enemyPos)
{
	obj.Update(camera);

	/*---�L�[����p---*/
	/*�ړ�*/
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

		if (input->Key(DIK_W))
		{
			pos.x += cosf(-rotY) * MOVE_SPEED;
			pos.z += sinf(-rotY) * MOVE_SPEED;

			camera.eye.x += cosf(-rotY) * MOVE_SPEED;
			camera.eye.z += sinf(-rotY) * MOVE_SPEED;
		}
		if (input->Key(DIK_S))
		{
			pos.x -= cosf(-rotY) * MOVE_SPEED;
			pos.z -= sinf(-rotY) * MOVE_SPEED;

			camera.eye.x -= cosf(-rotY) * MOVE_SPEED;
			camera.eye.z -= sinf(-rotY) * MOVE_SPEED;
		}

		obj.position = pos;
		camera.target = pos;
	}

	/*���Ԓ�~*/

	//���Ԓ�~�O
	if (!stopTimeFlag)
	{
		//CT�Ǘ�
		if (stopTImeDelay >= STOP_TIME_DELAY)
		{
			//���Ԓ�~��t
			if (input->KeyTrigger(DIK_E))
			{
				stopTImeDelay = 0;
				stopTimeFlag = true;
			}
		}
		else { stopTImeDelay++; }
	}

	//���Ԓ�~��
	if (stopTimeFlag)
	{
		//���t���[�����Ŏ��Ԓ�~����
		if (stopTimeCount >= STOP_TIME_COUNT)
		{
			stopTimeCount = 0;
			stopTimeFlag = false;
		}
		else { stopTimeCount++; }
	}

	/*�U��*/

	//�U���O
	if (!attackFlag)
	{
		if (input->KeyTrigger(DIK_SPACE) && attackDelay == 0) { attackFlag = true; }
		else if (attackDelay > 0) { attackDelay--; }
	}

	//�U����
	if (attackFlag)
	{
		//����
		//�G�ւ̕����x�N�g��
		XMFLOAT3 vec = { 0,0,0 };
		vec.x = enemyPos.x - obj.position.x;
		vec.z = enemyPos.z - obj.position.z;
		XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

		//���@�̕����x�N�g��
		XMFLOAT3 myVec = { 0,0,0 };
		float rad = angle;
		ConvertToRadian(rad);
		myVec.x = cos(rad);
		myVec.z = sin(rad);

		//�v�Z
		float dot = vec.x * myVec.x + vec.z * myVec.z;
		float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
		float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
		float cosTheta = dot / (absA * absB);
		float theta = acosf(cosTheta);
		ConvertToDegree(theta);
		//ConvertToDegree(dot);
		float attackAngle = theta;
		//float attackAngle = dot;

		//2�_�Ԃ̋���
		float diff = sqrt(
			(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
			(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
			(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));

		//���a�̍��v
		float r = 20 + 20;

		//�~�~�~
		if (attackAngle < ATTACK_ANGLE && diff < r) { hp = 0; }

		attackDelay = ATTACK_DELAY;
		attackFlag = false;
	}
}

void Player::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	//�K���ȃ��f���ő�p����
	if (hp > 0) { obj.modelDraw(model.GetModel(), pipelineSet); }
}

void Player::Finalize()
{
}
