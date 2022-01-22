#pragma once
#include "Input/DirectInput.h"
#include "3D/Model.h"
#include "3D/Object3D.h"
#include "ModelPipeline.h"

#include <DirectXMath.h>
#include <math.h>

const int STOP_TIME_COUNT = 60;						//Å‘åŽžŠÔ’âŽ~—Ê
const int STOP_TIME_DELAY = 30;						//UŒ‚‰Â”\‚Ü‚Å‚ÌƒN[ƒ‹ƒ^ƒCƒ€
const int ATTACK_DELAY = 30;						//UŒ‚‚ÌÅ‘åCT
const int MAX_HP = 30000000;								//HP
const float MOVE_SPEED = 1.0f;						//“®‚­‘¬‚³
const float MOVE_ANGLE = 3.0f;						//Œü‚«‚ð•Ï‚¦‚é‘¬‚³
const float ATTACK_ANGLE = 90;						//UŒ‚”ÍˆÍŠp“x

using namespace DirectX;

class Player
{
private:
	Input* input = Input::GetInstance();
	Model model;
	Object3D obj;
	Camera cam;

private:
	XMFLOAT3 pos;									//ƒ|ƒWƒVƒ‡ƒ“
	XMFLOAT3 vec3;									//Œü‚¢‚Ä‚¢‚é•ûŒü
	int hp;											//ƒqƒbƒgƒ|ƒCƒ“ƒg
	int stopTimeCount;								//ŽžŠÔ‚ðŽ~‚ß‚Ä‚¢‚éŠÔ‚ÌƒJƒEƒ“ƒg
	int stopTImeDelay;								//ŽžŠÔ’âŽ~‚ÌCT
	int attackCount;								//UŒ‚‚ÌƒN[ƒ‹ƒ^ƒCƒ€
	int attackDelay;								//UŒ‚‚ÌCT
	int drawCount;
	float angle;									//ˆÚ“®‚·‚éŠp“x
	bool attackFlag;								//UŒ‚‚µ‚Ä‚¢‚é‚©
	bool stopTimeFlag;								//ŽžŠÔŽ~‚ß‚Ä‚¢‚é‚©
	bool isHit;
	bool isDead;
	bool isDamaged;

public:
	Player();
	~Player();
	void Init(const Camera& camera);
	void Input(const Camera& camera);
	void Update(Camera& camera, const XMFLOAT3& enemyPos);
	void Draw(const PipeClass::PipelineSet& pipelineSet);
	void Finalize();
	void PushBack(const XMFLOAT3& enemyPos);

public:
	XMFLOAT3 GetPos() { return pos; }									//ƒ|ƒWƒVƒ‡ƒ“‚ð•Ô‚·
	//XMFLOAT3 GetDirection() { return direction; }						//Œü‚¢‚Ä‚é•ûŒü‚ð•Ô‚·
	XMFLOAT3 GetVec3() { return vec3; }									//•ûŒüƒxƒNƒgƒ‹‚ð•Ô‚·
	void SetVec3(XMFLOAT3 vec3) { this->vec3 = vec3; }					//•ûŒüƒxƒNƒgƒ‹‚ðƒZƒbƒg

	void Damaged()
	{
		isDamaged = true;
		if (hp > 0) { hp--; }
		else if (hp <= 0) { isDead = true; }
	}	//HP‚ðŒ¸‚ç‚·

	int GetHP() { return hp; }											//HP‚ð•Ô‚·
	int GetStopTimeCount() { return stopTimeCount; }
	int GetStopTimeDelay() { return stopTImeDelay; }
	float GetAngle() { return angle; }
	bool GetAttackFlag() { return attackFlag; }							//UŒ‚‚µ‚Ä‚¢‚é‚©‚ð•Ô‚·
	bool GetStopTimeFlag() { return stopTimeFlag; }						//ŽžŠÔ‚ðŽ~‚ß‚Ä‚¢‚é‚©Ž~‚ß‚Ä‚¢‚È‚¢‚©‚ð•Ô‚·
	bool IsHit() { return isHit; }										//“G‚ÉUŒ‚‚ª“–‚½‚Á‚½‚©
	bool IsDead() { return isDead; }									//Ž©‹@‚ªŽ€‚ñ‚¾‚©

private:
	void ConvertToRadian(float& degree)
	{
		degree = degree / 180.0f * XM_PI;
	}
	void ConvertToDegree(float& radian)
	{
		radian = radian * 180.0f / XM_PI;
	}
};

