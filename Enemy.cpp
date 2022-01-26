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
}

void Enemy::Init(const Camera& cam)
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

void Enemy::Generate(const Camera& cam, const XMFLOAT3& generatePos, const XMFLOAT3& forwardVec)
{
	//Init(cam);
	isAlive = true;
	this->enemyData.position = generatePos;
	this->forwardVec = forwardVec;
}

void Enemy::Update(const XMFLOAT3& playerPos, const float& angle, const bool& isAttack, const bool& isStop)
{
	//�����ς݂���Ȃ������琶������
	if (!isAlive)return;

	if (isAttack)
	{
		for (int i = 0; i < MAX_BULLET; ++i)
		{
			if (!enemyBullet[i].isAlive)continue;
			//����
			//�G�ւ̕����x�N�g��
			XMFLOAT3 vec = { 0,0,0 };
			//vec.x = enemyPos.x - obj.position.x;
			vec.x = playerPos.x - enemyBullet[i].bulletData.position.x;
			vec.z = playerPos.z - enemyBullet[i].bulletData.position.z;
			XMStoreFloat3(&vec, XMVector3Normalize(XMLoadFloat3(&vec)));

			//���@�̕����x�N�g��
			XMFLOAT3 myVec = { 0,0,0 };
			float rad = angle;
			XMConvertToRadians(rad);
			myVec.x = cosf(rad);
			myVec.z = sinf(rad);

			//�v�Z
			float dot = vec.x * myVec.x + vec.z * myVec.z;
			float absA = sqrtf(vec.x * vec.x + vec.z * vec.z);
			float absB = sqrtf(myVec.x * myVec.x + myVec.z * myVec.z);
			float cosTheta = dot / (absA * absB);
			float theta = acosf(-cosTheta);

			XMConvertToDegrees(theta);
			//ConvertToDegree(dot);
			float attackAngle = theta;
			//float attackAngle = dot;

			//2�_�Ԃ̋���
			float distance = Distance3D(playerPos, enemyBullet[i].bulletData.position);

			//���a�̍��v
			float r = 15.0f + enemyBullet->BULLET_RADIUS;;

			//�~�~�~
			if (attackAngle < 90 && distance < r)
			{
				enemyBullet[i].status = enemyBullet->BULLET_STATUS_EXPLOSION;
				enemyBullet[i].bulletData.color = { 49,78,97,255 };
			}
		}
	}

	if (isStop)return;

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
		Attack();
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


	//���C�̍X�V����
	//���W
	forwardRay.start = XMLoadFloat3(&enemyData.position);
	//����
	forwardRay.dir = XMLoadFloat3(&forwardVec);

	//�����W�̍X�V
	sphere.center = XMLoadFloat3(&enemyData.position);

	//�O�p�`���W�̍X�V
	forwardTriangle.p0 = XMLoadFloat3(&enemyData.position);
	
	//�E��
	XMFLOAT3 p1 = enemyData.position;
	p1 = AddXMFLOAT3(p1, TRIANGLE_UPPER_RIGHT_POS);
	p1 = MulXMFLOAT3(p1, forwardVec);
	p1.y = 10;
	forwardTriangle.p1 = XMLoadFloat3(&p1);

	//����
	XMFLOAT3 p2 = enemyData.position;
	p2 = AddXMFLOAT3(p2, TRIANGLE_UPPER_LEFT_POS);
	p2 = MulXMFLOAT3(p2, forwardVec);
	p2.y = 10;
	forwardTriangle.p2 = XMLoadFloat3(&p2);
}

void Enemy::BulletUpdate(const Camera &cam)
{
	//�e�̍X�V
	for (int i = 0; i < MAX_BULLET; ++i)
	{
		enemyBullet[i].Update();
	}
}

void Enemy::Draw(const PipeClass::PipelineSet& pipelineSet, const ModelObject& model)
{
	if (isAlive)
	{
		enemyData.modelDraw(model, pipelineSet);
	}
}

//void Enemy::Searching(const Sphere& playerSphere)
//{
//	searchTimer++;
//
//	if (searchTimer >= MAX_SEARCH_TIMER)
//	{
//		//rotateStatus = GetRand(0, 4);
//		rotateStatus = 0;
//		searchTimer = 0;
//	}
//
//	switch (rotateStatus)
//	{
//	case ROTATE_STATUS_RIGHT:
//		enemyData.rotation.y += ROTATE_SPEED;
//		break;
//	case ROTATE_STATUS_LEFT:
//		enemyData.rotation.y -= ROTATE_SPEED;
//		break;
//	case ROTATE_STATUS_STOP:
//		break;
//	default:
//		enemyData.rotation.y += ROTATE_SPEED;
//		break;
//	}
//
//	//searchDelayTimer++;
//
//
//	XMFLOAT3 playerPos = {};
//	XMStoreFloat3(&playerPos, playerSphere.center);
//	//���ʃ��C�����ɕǂ��Ȃ��A�G��������
//	if (Collision::CheckRay2Sphere(forwardRay, playerSphere) && !CheckRay2Walls(forwardRay, WallMgr::Instance()->GetWalls(), playerPos))
//	{
//		//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
//		status = STATUS_TARGET;
//		searchTimer = 30;
//		searchDelayTimer = 0;
//	}
//
//	
//	//if (Collision::CheckSphere2Triangle(playerSphere, forwardTriangle))
//	//{
//	//	//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
//	//	status = STATUS_TARGET;
//	//	searchTimer = 30;
//	//	searchDelayTimer = 0;
//	//}
//}

//void Enemy::Targeting(const XMFLOAT3& playerPos)
//{
//	XMFLOAT3 buff = XMFLOAT3(enemyData.position.x - playerPos.x, enemyData.position.y - playerPos.y, enemyData.position.z - playerPos.z);
//	//���ʃx�N�g�����v���C���[�̕����Ɍ�����
//	forwardVec = Normalize3D(buff);
//
//	//�����Ă������
//	XMFLOAT3 distance = {};
//	UpdateForwardVec(distance, matRot);
//
//	float angle = Cross2D({ forwardVec.x,forwardVec.z }, { distance.x,distance.z });
//
//	if (angle < 0)
//	{
//		enemyData.rotation.y += XM_PI / 2.0f;
//	}
//	else
//	{
//		enemyData.rotation.y -= XM_PI / 2.0f;
//	}
//
//	//�ǔ����ɕǂɉB���ꂽ��X�e�[�^�X��{���ɕς���
//	if (CheckRay2Walls(forwardRay, WallMgr::Instance()->GetWalls(), playerPos))
//	{
//		status = STATUS_SEARCH;
//	}
//
//	//�^�C�}�[��i�߂�
//	targetingTimer++;
//
//	if (targetingTimer >= TARGET_TIMER_END)
//	{
//		targetingTimer = 0;
//		//�U���J�n���̃v���C���[���W��ۑ�
//		prevPlayerPos = playerPos;
//
//		isAttack = true;
//
//		//enemyBullet.Generate(enemyData.position, forwardVec);
//
//		//�X�e�[�^�X���U���ɂ���
//		status = STATUS_ATTACK;
//	}
//}

void Enemy::Attack()
{
	bulletTimer++;

	if (bulletTimer > MAX_BULLET_TIMER)
	{
		bulletTimer = 0;
		for (int i = 0; i < MAX_BULLET; i++)
		{
			if (enemyBullet[i].isAlive)continue;
			enemyBullet[i].Generate(enemyData.position, forwardVec);
			break;
		}
	}

	//isAttack = false;
	
	//attackDelayTimer++;

	//if (attackDelayTimer >= ATTACK_DELAY_TIMER_END)
	//{
	//	attackDelayTimer = 0;
	//	status = STATUS_SEARCH;
	//}
}

void Enemy::UpdateForwardVec(XMFLOAT3& forwardVec, XMMATRIX& matRot)
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

bool Enemy::CheckRay2Walls(const Ray& ray, std::vector<Wall>& walls, const XMFLOAT3& playerPos)
{
	//���ʃ��C�����ɕǂ���������
	//�ǈ���
	for (int i = 0; i < walls.size(); ++i)
	{
		XMFLOAT3 wallPos = walls[i].GetPos();
		if (Distance3D(enemyData.position, wallPos) > Distance3D(enemyData.position, playerPos)) continue;

		//��1�̎O�p�`�Ԃ��
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

Sphere Enemy::GetNearEnemyBulletSphere(const XMFLOAT3& playerPos)
{
	Sphere saveSphere = {};
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
			saveSphere = enemyBullet[i].bulletSphere;
		}
	}
	return saveSphere;
}

void Enemy::Dead()
{
	isAlive = false;
	//for (int i = 0; i < MAX_BULLET; i++)
	//{
	//	enemyBullet[i].Dead();
	//}
}
