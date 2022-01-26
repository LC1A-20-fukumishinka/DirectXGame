#pragma once
#include "particleManager.h"
#include "Vector3.h"
class BombEffect
{
public:
	BombEffect();
	~BombEffect();
	void Init();
	void Update();
	void Finalize();
	void Draw();

	void Bomb(const DirectX::XMFLOAT3 &pos, float speed);

	static void SetTexture(int texNum);
private:

ParticleManager *bombParticle;

static int textureNumber;
};