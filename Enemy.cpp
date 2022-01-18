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
	//�����ς݂���Ȃ������琶������
	if (!isAlive)return;

	//�X�V����
	enemyData.Update(cam);

	//�X�e�[�^�X�ɂ���ď����𕪂���
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

	//���ʃx�N�g���̍X�V
	XMMATRIX matRot = XMMatrixIdentity();
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

	////���ʃ��C�����ɓG��������
	//if (Collision::CheckRay2Sphere(forwardRay, playerSphere) && searchDelayTimer >= SEARCH_DELAY_TIMER_END)
	//{
	//	//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
	//	status = STATUS_TARGET;
	//	searchTimer = 30;
	//	searchDelayTimer = 0;
	//}

	
	if (Collision::CheckSphere2Triangle(playerSphere, forwardTriangle))
	{
		//�X�e�[�^�X���^�[�Q�e�B���O�ɂ���
		status = STATUS_TARGET;
		searchTimer = 30;
		searchDelayTimer = 0;
	}
}

void Enemy::Targeting(const XMFLOAT3& playerPos)
{
	XMFLOAT3 buff = XMFLOAT3(enemyData.position.x - playerPos.x, enemyData.position.y - playerPos.y, enemyData.position.z - playerPos.z);
	//���ʃx�N�g�����v���C���[�̕����Ɍ�����
	forwardVec = Normalize3D(buff);

	//�^�C�}�[��i�߂�
	targetingTimer++;

	if (targetingTimer >= TARGET_TIMER_END)
	{
		targetingTimer = 0;
		//�U���J�n���̃v���C���[���W��ۑ�
		prevPlayerPos = playerPos;

		isAttack = true;

		//�X�e�[�^�X���U���ɂ���
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
