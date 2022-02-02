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
	//�����ς݂���Ȃ������琶������
	if (!isAlive)return;

	//�G�̃X�e�[�^�X�ɂ���ď����𕪂���
	if (!isEnemyTypeSeach)
	{
		if (isStop)
		{
			XMFLOAT3 nearBulletPos = GetNearEnemyBulletPos(playerPos);
			if (isAttack)
			{
				if (enemyBullet[saveNum].isAlive)
				{
					//����
					//�G�ւ̕����x�N�g��
					XMFLOAT3 vec = { 0,0,0 };
					//vec.x = enemyPos.x - obj.position.x;
					vec.x = playerPos.x - nearBulletPos.x;
					vec.z = nearBulletPos.z - playerPos.z;
					XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

					//���@�̕����x�N�g��
					XMFLOAT3 myVec = { 0,0,0 };
					float rad = angle;
					rad = XMConvertToRadians(rad);
					myVec.x = cosf(rad);
					myVec.z = sinf(rad);

					//�v�Z
					float dot = vec.x * myVec.x + vec.z * myVec.z;
					float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
					float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
					float cosTheta = dot / (absA * absB);
					float theta = acosf(-cosTheta);

					theta = XMConvertToDegrees(theta);
					//ConvertToDegree(dot);
					float attackAngle = theta;
					//float attackAngle = dot;

					//2�_�Ԃ̋���
					float distance = Distance3D(playerPos, nearBulletPos);

					//���a�̍��v
					float r = 30.0f + enemyBullet->BULLET_RADIUS;

					//�~�~�~
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
			//�e�̏���
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
					//2�_�Ԃ̋��������߂�
					float distance = Distance3D(enemyPos1, enemyPos2);
					//���a�̘a�����߂�
					float r = enemyBullet[i].EXPLOSION_RADIUS + enemyBullet[j].BULLET_RADIUS;
					if (distance < r)
					{
						enemyBullet[j].status = enemyBullet->BULLET_STATUS_EXPLOSION;
					}
				}
				//�G�Ƃ̋��������߂�
				float distance = Distance3D(enemyPos1, enemyData.position);
				float r = enemyBullet[i].EXPLOSION_RADIUS + ENEMY_RADIUS;
				if (distance < r)
				{
					Dead();
				}
			}

			//�e�̏���
			for (int i = 0; i < MAX_BULLET; ++i)
			{
				if (!enemyBullet[i].isAlive)continue;
				if (enemyBullet[i].status != enemyBullet->BULLET_STATUS_ALIVE)continue;

				XMFLOAT3 enemyPos1 = enemyBullet[i].bulletData.position;

				//�v���C���[�Ƃ̓����蔻��
				float pDistance = Distance3D(enemyPos1, playerPos);
				float playerRadius = 8.0f;
				float pR = enemyBullet->BULLET_RADIUS + playerRadius;
				if (pDistance < pR)
				{
					isHit = true;
				}
			}

			status = STATUS_ATTACK;

			//�X�e�[�^�X�ɂ���ď����𕪂���
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

			//���ʃx�N�g���̍X�V
			matRot = XMMatrixIdentity();
			matRot *= XMMatrixRotationZ(XMConvertToRadians(enemyData.rotation.z));
			matRot *= XMMatrixRotationX(XMConvertToRadians(enemyData.rotation.x));
			matRot *= XMMatrixRotationY(XMConvertToRadians(enemyData.rotation.y));
			UpdateForwardVec(forwardVec, matRot);

			//�X�P�[���̍X�V
			enemyData.scale = { scale, scale, scale };

			//���C�̍X�V����
			//���W
			forwardRay.start = XMLoadFloat3(&enemyData.position);
			//����
			forwardRay.dir = XMLoadFloat3(&forwardVec);

			//�����W�̍X�V
			sphere.center = XMLoadFloat3(&enemyData.position);
		}
	}
	else//����������G����G�̏���
	{
	//���Ԓ�~���Ă����珈���͒ʂ�Ȃ�
		if (!isStop)
		{
			isHit = false;
			//�X�e�[�^�X�ɂ���ď����𕪂���
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

			//���ʃx�N�g���̍X�V
			matRot = XMMatrixIdentity();
			matRot *= XMMatrixRotationZ(XMConvertToRadians(enemyData.rotation.z));
			matRot *= XMMatrixRotationX(XMConvertToRadians(enemyData.rotation.x));
			matRot *= XMMatrixRotationY(XMConvertToRadians(enemyData.rotation.y));
			UpdateForwardVec(forwardVec, matRot);

			//�X�P�[���̍X�V
			enemyData.scale = { scale, scale, scale };

			//���C�̍X�V����
			//���W
			forwardRay.start = XMLoadFloat3(&enemyData.position);
			//����
			forwardRay.dir = XMLoadFloat3(&forwardVec);

			//�����W�̍X�V
			sphere.center = XMLoadFloat3(&enemyData.position);
		}
		else
		{
			//���Ԓ�~���Ă�����ǔ�SE��炳�Ȃ�
			targetSE->Stop();
		}
	}
}


void Enemy::BulletUpdate()
{
	//�e�̍X�V
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

	//���G���͈������ɉ�]��������
	enemyData.rotation.y += ROTATE_SPEED;

	scale = 10.0f;

	//�����蔻��Ɏg���v���C���[�̓����蔻��
	Sphere playerSphere = {};
	playerSphere.center = XMLoadFloat3(&playerPos);
	playerSphere.radius = 16;

	//�G�ƃv���C���[�̋������v�Z
	float distance = Distance3D(enemyData.position, playerPos);
	//�G�ƃv���C���[�̔��a�̍��v
	float r = 16 + SEARCH_RADIUS;

	//�G�̊��m�͈͂ƃv���C���[�̔��a�̍��v
	float sR = 16 + SENSING_RADIUS;

	//������̋����ȏ�߂��Ƀv���C���[��������
	if (distance < sR)
	{
		//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
		status = STATUS_TARGET;
		targetSE->PlayLoop();
		searchTimer = 30;
		searchDelayTimer = 0;
	}

	//���ʃ��C�����ɕǂ��Ȃ��A�G��������
	if (Collision::CheckRay2Sphere(forwardRay, playerSphere) && !CheckRay2Walls(forwardRay, WallMgr::Instance()->GetWalls(), playerPos) && distance < r)
	{
		//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
		status = STATUS_TARGET;
		targetSE->PlayLoop();
		searchTimer = 30;
		searchDelayTimer = 0;
	}


	//if (Collision::CheckSphere2Triangle(playerSphere, forwardTriangle))
	//{
	//	//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
	//	status = STATUS_TARGET;
	//	searchTimer = 30;
	//	searchDelayTimer = 0;
	//}
}

void Enemy::Targeting(const XMFLOAT3 &playerPos)
{
	//�G����v���C���[�ւ̕����x�N�g��
	XMFLOAT3 buff = XMFLOAT3(enemyData.position.x - playerPos.x, enemyData.position.y - playerPos.y, enemyData.position.z - playerPos.z);
	//���ʃx�N�g�����v���C���[�̕����Ɍ�����
	forwardVec = Normalize3D(buff);

	//�G�̌����Ă����������]�s�񂩂�擾����
	XMFLOAT3 directionVec = {};
	UpdateForwardVec(directionVec, matRot);

	//�G�̌����Ă���x�N�g���ƃv���C���[�ւ̃x�N�g���̊O�ς��v�Z����
	float cross = Cross2D({ forwardVec.x,forwardVec.z }, { directionVec.x,directionVec.z });

	//�G�̌����Ă���x�N�g���ƃv���C���[�ւ̃x�N�g���̊p�x���v�Z����
	float angle = calAngle(forwardVec, directionVec);
	angle = acosf(angle);
	angle = 180.0f - fabs(XMConvertToDegrees(angle));

	//�������̂������������Ƃ�(angle��nan�̂Ƃ�)�͉�]�������Ȃ�
	if (!std::isnan(angle))
	{
		if (cross < 0)
		{
			//�p�x����]�p�x��菬����������p�x����]������
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
			//�p�x����]�p�x��菬����������p�x����]������
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

	//�G�ƃv���C���[�̋������v�Z
	float dis = Distance3D(enemyData.position, playerPos);
	//�G�̊��m�͈͂ƃv���C���[�̔��a�̍��v
	float sR = 16 + SENSING_RADIUS;

	//���m�͈͊O���A�G�̌�둤�������ꍇ�������悤�ɂ���
	if (cross > 0.75 && cross < 1 && dis > sR)
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//���~�߂�
		targetSE->Stop();
	}
	else if (cross < -0.75 && cross > -1 && dis > sR)
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//���~�߂�
		targetSE->Stop();
	}

	//�G����v���C���[�Ɍ����Ẵ��C
	Ray honraiRay = {};
	honraiRay.start = XMLoadFloat3(&enemyData.position);
	honraiRay.dir = XMLoadFloat3(&(playerPos - enemyData.position));

	//�ǔ����ɕǂɉB���ꂽ��X�e�[�^�X��{���ɕς���
	if (CheckRay2Walls(honraiRay, WallMgr::Instance()->GetWalls(), playerPos))
	{
		status = STATUS_SEARCH;
		targetingTimer = 0;
		//���~�߂�
		targetSE->Stop();
	}

	//�X�P�[�������Z����
	scale += addScale;

	if (scale >= 12.0f || scale <= 8.0f)
	{
		addScale *= -1;
	}

	//�^�C�}�[��i�߂�
	targetingTimer++;

	//�^�C�}�[���K��l�ɒB������U��������悤�ɂ���
	if (targetingTimer >= TARGET_TIMER_END)
	{
		targetingTimer = 0;
		//�U���J�n���̃v���C���[���W��ۑ�
		prevPlayerPos = playerPos;

		isAttack = true;
		//�ˌ��T�E���h���Đ�
		shotSE->Play();

		//�ǔ��T�E���h���~
		targetSE->Stop();

		//�X�e�[�^�X���U���ɂ���
		status = STATUS_ATTACK;
	}
}

void Enemy::Attack(const XMFLOAT3 &playerPos)
{
	if (!isEnemyTypeSeach)
	{
		//�e��A�˂�����
		bulletTimer++;

		if (bulletTimer > MAX_BULLET_TIMER)
		{
			bulletTimer = 0;
			for (int i = 0; i < MAX_BULLET; i++)
			{
				//�������Ă���ꍇ�������΂�
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
	//���ʃx�N�g���̃t�H�[�}�b�g
	XMFLOAT3 honraiForwardVec = { 0,0,1 };
	XMVECTOR honraiForwardVector = XMLoadFloat3(&honraiForwardVec);

	//���ʃx�N�g���̃t�H�[�}�b�g�Ɏ��@�̉�]�s���������
	honraiForwardVector = XMVector3Transform(honraiForwardVector, matRot);

	//���������ʃx�N�g���𐳋K������
	honraiForwardVector = XMVector3Normalize(honraiForwardVector);

	//���ʃx�N�g�����X�V����
	XMStoreFloat3(&forwardVec, honraiForwardVector);
}

bool Enemy::CheckRay2Walls(const Ray &ray, std::vector<Wall> &walls, const XMFLOAT3 &playerPos)
{
	//���ʃ��C�����ɕǂ���������
	//�ǈ���
	for (int i = 0; i < walls.size(); ++i)
	{
		//��1�̎O�p�`�Ԃ��
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
		//�������Ă�G�����v�Z����
		if (!enemyBullet[i].isAlive)continue;
		//�v���C���[�Ƃ̋����𑪂�
		float distance = Distance3D(playerPos, enemyBullet[i].bulletData.position);
		//�b��ŏ���菬����������
		if (saveDistance >= distance)
		{
			//�ŏ����X�V����
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
