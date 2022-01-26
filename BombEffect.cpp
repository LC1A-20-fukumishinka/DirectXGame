#include "BombEffect.h"

int BombEffect::textureNumber = -1;

BombEffect::BombEffect()
{
	bombParticle = ParticleManager::Create();
}

BombEffect::~BombEffect()
{
	delete bombParticle;
}

void BombEffect::Init()
{
}

void BombEffect::Update()
{
	bombParticle->Update();
}

void BombEffect::Finalize()
{
}

void BombEffect::Draw()
{
	bombParticle->Draw(textureNumber);
}

void BombEffect::Bomb(const DirectX::XMFLOAT3 &pos, float speed)
{

	for (int i = 0; i < 20; i++)
	{
		Vector3 angle((float)rand() / RAND_MAX * 2 - 1, 0, (float)rand() / RAND_MAX * 2 - 1);

		angle = angle.normalaize();
		bombParticle->Add(60, pos, angle * speed, Vector3(), 15, 5, DirectX::XMFLOAT4 {0.3f, 0, 0, 1}, DirectX::XMFLOAT4{ 0.3, 0, 0, 1 });
	}
}

void BombEffect::SetTexture(int texNum)
{
	textureNumber = texNum;
}
