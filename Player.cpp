#include "Player.h"
#include "Shake.h"

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
	movePower = 0;
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

void Player::Init(const Camera& camera, const XMFLOAT3& pos)
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
	movePower = 0;
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

void Player::Input(const Camera& camera)
{
	/*---�R���g���[���[����p---*/
	if (input->isPadConnect())
	{
		//�R���g���[���[�̕����x�N�g��
		XMFLOAT3 contVec = { 0,0,0 };
		contVec.x = input->LStick().x;
		contVec.z = input->LStick().y;

		//���K��
		XMStoreFloat3(&contVec, XMVector3Normalize(XMLoadFloat3(&contVec)));

		//���͂�����ꍇ
		if (contVec.x != 0 && contVec.z != 0)
		{
			//�ړ����炩�p
			if (movePower < 1.0f) movePower += 0.04;

			//�����̕����x�N�g��
			XMFLOAT3 myVec = { 0,0,0 };
			float rotY = obj.rotation.y - 90.0f;
			ConvertToRadian(rotY);
			myVec.x = cosf(-rotY);
			myVec.z = sinf(-rotY);

			//�R���g���[���[�̃A���O��
			float contAngle = -atan2f(contVec.z, contVec.x);
			ConvertToDegree(contAngle);
			if (contAngle < 0.0f) { contAngle += 360.0f; }

			//�����̃A���O��
			float myAngle = -atan2f(myVec.z, myVec.x);
			ConvertToDegree(myAngle);
			if (myAngle < 0.0f) { myAngle += 360.0f; }

			//�_�b�V�����͂��������ꍇ
			if (input->ButtonTrigger(XINPUT_GAMEPAD_B))
			{
				myAngle = contAngle;
			}

			//�߂����𔻒�
			float hosei = RotateEarliestArc(myAngle, contAngle);

			//�v�Z
			myAngle = myAngle + hosei / 10.0f;
			obj.rotation.y = myAngle + 90.0f;
			angle = myAngle;
			ConvertToRadian(myAngle);
			myVec.x = cosf(-myAngle);
			myVec.z = sinf(-myAngle);
			vec3 = myVec;
		}
		else {
			if (movePower > 0.0f) movePower -= 0.06f;
			if (movePower < 0.0f) movePower = 0.0f;
		}
	}


	/*---�L�[����p---*/
	else
	{
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

	//�f�o�b�O�p
	if (input->KeyTrigger(DIK_DOWN))
	{
		Damaged();
	}
}

void Player::Update(Camera& camera, const XMFLOAT3& enemyPos)
{
	if (!isDead)
	{
		if (input->isPadConnect())
		{
			//�_�b�V�����͂��������ꍇ
			if (input->ButtonTrigger(XINPUT_GAMEPAD_B))
			{
				pos.x += vec3.x * DASH_SPEED;
				pos.z += vec3.z * DASH_SPEED;
			}
			else
			{
				pos.x += vec3.x * MOVE_SPEED * movePower;
				pos.z += vec3.z * MOVE_SPEED * movePower;
			}
		}
		else
		{
			pos.x += vec3.x;
			pos.z += vec3.z;
		}
	}

	attackFlag = false;
	camera.position = pos;
	obj.position = pos;
	obj.Update(camera);

	/*---�L�[����p---*/
	/*���Ԓ�~*/

	//���Ԓ�~�O
	if (!stopTimeFlag)
	{
		//CT�Ǘ�
		if (stopTImeDelay >= STOP_TIME_DELAY)
		{
			if (input->isPadConnect())
			{
				//���Ԓ�~��t
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

	//�U����
	if (attackFlag)
	{
		//����
		//�G�ւ̕����x�N�g��
		XMFLOAT3 vec = { 0,0,0 };
		//vec.x = enemyPos.x - obj.position.x;
		vec.x = obj.position.x - enemyPos.x;
		vec.z = enemyPos.z - obj.position.z;
		XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

		//���@�̕����x�N�g��
		XMFLOAT3 myVec = { 0,0,0 };
		float rad = angle;
		ConvertToRadian(rad);
		myVec.x = cosf(rad);
		myVec.z = sinf(rad);

		//�v�Z
		float dot = vec.x * myVec.x + vec.z * myVec.z;
		float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
		float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
		float cosTheta = dot / (absA * absB);
		float theta = acosf(-cosTheta);

		ConvertToDegree(theta);
		//ConvertToDegree(dot);
		float attackAngle = theta;
		//float attackAngle = dot;

		//2�_�Ԃ̋���
		float dist = sqrtf(
			(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
			(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
			(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));

		//���a�̍��v
		float r = 15.0f + 32.0f;

		//�~�~�~
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

	//��_���[�W���V�F�C�N
	if (isDamaged)
	{
		if (!isDead)
		{
			camera.SetShift(Shake::GetShake(1.0f, true, false, true));
			damagedCount++;
			if (damagedCount > INVINCIBLE_COUNT)
			{
				isDamaged = false; damagedCount = 0;
			}
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

void Player::Draw(const PipeClass::PipelineSet& pipelineSet)
{
	//�K���ȃ��f���ő�p����
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

void Player::PushBack(const XMFLOAT3& enemyPos)
{
	//���a
	float r = 13.0f + 14.0f;
	//2�_�Ԃ̋���
	/*float dist = sqrtf(
		(enemyPos.x - obj.position.x) * (enemyPos.x - obj.position.x) +
		(enemyPos.y - obj.position.y) * (enemyPos.y - obj.position.y) +
		(enemyPos.z - obj.position.z) * (enemyPos.z - obj.position.z));*/
	float dist = sqrtf(
		(enemyPos.x - pos.x) * (enemyPos.x - pos.x) +
		(enemyPos.y - pos.y) * (enemyPos.y - pos.y) +
		(enemyPos.z - pos.z) * (enemyPos.z - pos.z));
	//�d�Ȃ����ꍇ
	if (dist < r)
	{
		//���@�ւ̕����x�N�g��
		XMFLOAT3 vec3 = { 0,0,0 };
		vec3.x = obj.position.x - enemyPos.x;
		vec3.z = obj.position.z - enemyPos.z;
		XMStoreFloat3(&vec3, XMVector3Normalize(XMLoadFloat3(&vec3)));

		pos.x = enemyPos.x + vec3.x * r;
		pos.z = enemyPos.z + vec3.z * r;
	}
}

void Player::DeathEffect(Camera& camera)
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

bool Player::SetGoalAndCheak(const XMFLOAT3& lowerLeft, const XMFLOAT3& upperRight)
{
	if (lowerLeft.x <= pos.x <= upperRight.x &&
		lowerLeft.z <= pos.z <= upperRight.z)
	{
		return true;
	}
	return false;
}

void Player::ClearEffect(Camera& camera, bool setGoalAndCheak)
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

