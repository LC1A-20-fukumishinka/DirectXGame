#include "Wall.h"
#include "ModelPipeline.h"
using namespace DirectX;

const DirectX::XMFLOAT3 &operator -(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2);
const DirectX::XMFLOAT3 &operator +(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2);
const DirectX::XMFLOAT3 &operator *(const DirectX::XMFLOAT3 v1, const float s);

Model *Wall::wallModel = nullptr;

const float Length(const DirectX::XMFLOAT3 v);

Wall::Wall()
{
}

Wall::~Wall()
{
}

void Wall::Init(Camera &cam, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 collisionScale)
{
	this->cam = &cam;
	wallObj.position = pos;
	wallObj.scale = scale;
	//wallObj.color.w = 0.5f;
	wallObj.color.w = 1.0f;
	wallObj.Init(*this->cam);
	this->collisionScale = collisionScale;
}

void Wall::Update()
{
	wallObj.Update(*this->cam);
}

void Wall::Draw()
{
	wallObj.modelDraw(wallModel->GetModel(), ModelPipeline::GetInstance()->GetPipeLine());
}

void Wall::Finalize()
{
}

void Wall::SetModel(Model &model)
{
	wallModel = &model;
}

bool Wall::LineCollision(DirectX::XMFLOAT3 wallA, DirectX::XMFLOAT3 wallB, DirectX::XMFLOAT3 playerA, DirectX::XMFLOAT3 playerB)
{
	XMFLOAT3  wallLine = wallA - wallB,
		playerLine = playerA - playerB;

	//if ((wallLine.cross(Bpos1 - Apos0) * wallLine.cross(Bpos0 - Apos0) < 0) &&
		//	(playerLine.cross(Apos1 - Bpos0) * playerLine.cross(Apos0 - Bpos0) < 0))
	if ((LineCross(wallLine, playerB - wallA) * LineCross(wallLine, playerA - wallA) < 0) &&
		(LineCross(playerLine, wallB - playerA) * LineCross(playerLine, wallA - playerA) < 0))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

float Wall::LineCross(DirectX::XMFLOAT3 wallLine, DirectX::XMFLOAT3 playerLine)
{

	return (wallLine.x * playerLine.z) - (wallLine.z * playerLine.x);
}

DirectX::XMFLOAT3 Wall::PushBack(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 size, DirectX::XMFLOAT3 MoveSpeed)
{
	//�����̓����蔻��Ɏg��������
	//XMFLOAT3 CenPos = { this->pos - size },
	//	CenSize = { this->size + size };

	XMFLOAT3 CenPos = wallObj.position,
		CenSize = collisionScale + size;

	//�l�p�̎l���̓_
	XMFLOAT3 LN = { CenPos.x - CenSize.x, CenPos.y , CenPos.z - CenSize.z };
	XMFLOAT3 RN = { CenPos.x + CenSize.x, CenPos.y , CenPos.z - CenSize.z };
	XMFLOAT3 LF = { CenPos.x - CenSize.x, CenPos.y , CenPos.z + CenSize.z };
	XMFLOAT3 RF = { CenPos.x + CenSize.x, CenPos.y , CenPos.z + CenSize.z };
	//�v���C���[�̈ړ���̍��W
	XMFLOAT3 AfterPos = { pos + MoveSpeed };

	bool up = false,
		down = false,
		right = false,
		left = false;

	//Vector2 upP, downP, rightP, leftP;

	//�ŏI�I�Ƀv���C���[�𓮂����x�N�g��
	XMFLOAT3 moveP = {};

	float length = 1000.0f;
	up = LineCollision(LN, RN, pos, AfterPos);

	down = LineCollision(LF, RF, pos, AfterPos);

	left = LineCollision(LN, LF, pos, AfterPos);

	right = LineCollision(RN, RF, pos, AfterPos);
	//��
	if (up)
	{
		XMFLOAT3 line = RN - LN;

		float lineLength = Length(line);
		float d0 = fabs(LineCross(line, pos - LN));
		float d1 = fabs(LineCross(line, AfterPos - LN));

		float rate = d0 / (d0 + d1);

		XMFLOAT3 interSection = pos + (MoveSpeed * rate);
		XMFLOAT3 dist = interSection - pos;

		//�����O�̍��W�ƌ�_�����̓_���߂����������ւ���
		if (length > Length(dist));
		{
			length = Length(dist);
			moveP = { 0, 0,-d1 / lineLength - 0.1f };
		}
	}
	//��
	if (down)
	{
		XMFLOAT3 line = RF - LF;

		float lineLength = Length(line);
		float d0 = fabs(LineCross(line, pos - LF));
		float d1 = fabs(LineCross(line, AfterPos - LF));

		float rate = d0 / (d0 + d1);

		XMFLOAT3 interSection = pos + (MoveSpeed * rate);
		XMFLOAT3 dist = interSection - pos;

		//�����O�̍��W�ƌ�_�����̓_���߂����������ւ���
		if (length > Length(dist));
		{
			length = Length(dist);
			moveP = { 0,0.0f,  d1 / lineLength + 0.1f };
		}
	}
	//��
	if (left)
	{
		XMFLOAT3 line = LF - LN;

		float lineLength = Length(line);
		float d0 = fabs(LineCross(line, pos - LN));
		float d1 = fabs(LineCross(line, AfterPos - LN));

		float rate = d0 / (d0 + d1);

		XMFLOAT3 interSection = pos + (MoveSpeed * rate);
		XMFLOAT3 dist = interSection - pos;

		//�����O�̍��W�ƌ�_�����̓_���߂����������ւ���
		if (length > Length(dist));
		{
			length = Length(dist);
			moveP = { -d1 / lineLength - 0.1f, 0.0f, 0 };
		}
	}
	//�E
	if (right)
	{
		XMFLOAT3 line = RF - RN;

		float lineLength = Length(line);
		float d0 = fabs(LineCross(line, pos - RN));
		float d1 = fabs(LineCross(line, AfterPos - RN));

		float rate = d0 / (d0 + d1);

		XMFLOAT3 interSection = pos + (MoveSpeed * rate);
		XMFLOAT3 dist = interSection - pos;

		//�����O�̍��W�ƌ�_�����̓_���߂����������ւ���
		if (length > Length(dist));
		{
			length = Length(dist);
			moveP = { d1 / lineLength + 0.1f,0, 0 };
		}
	}
	return moveP;
}

const XMFLOAT3 &operator-(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	return XMFLOAT3{ v1.x - v2.x,v1.y - v2.y ,v1.z - v2.z };
}

const DirectX::XMFLOAT3 &operator+(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	return XMFLOAT3{ v1.x + v2.x,v1.y + v2.y ,v1.z + v2.z };
}

const DirectX::XMFLOAT3 &operator*(const DirectX::XMFLOAT3 v1, const float s)
{
	return XMFLOAT3{ v1.x * s,v1.y * s ,v1.z * s };
}

const float Length(const DirectX::XMFLOAT3 v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
