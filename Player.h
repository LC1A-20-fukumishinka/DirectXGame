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
const int MAX_HP = 3;								//HP
const float MOVE_SPEED = 1.0f;						//“®‚­‘¬‚³
const float MOVE_ANGLE = 3.0f;						//Œü‚«‚ð•Ï‚¦‚é‘¬‚³
const float ATTACK_ANGLE = 60;						//UŒ‚”ÍˆÍŠp“x

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
	XMFLOAT3 direction;								//Œü‚¢‚Ä‚¢‚é•ûŒü
	int hp;											//ƒqƒbƒgƒ|ƒCƒ“ƒg
	int stopTimeCount;								//ŽžŠÔ‚ðŽ~‚ß‚Ä‚¢‚éŠÔ‚ÌƒJƒEƒ“ƒg
	int stopTImeDelay;								//ŽžŠÔ’âŽ~‚ÌCT
	int attackCount;								//UŒ‚‚ÌƒN[ƒ‹ƒ^ƒCƒ€
	int attackDelay;								//UŒ‚‚ÌCT
	float angle;									//ˆÚ“®‚·‚éŠp“x
	bool attackFlag;								//UŒ‚‚µ‚Ä‚¢‚é‚©
	bool stopTimeFlag;								//ŽžŠÔŽ~‚ß‚Ä‚¢‚é‚©

public:
	Player();
	~Player();
	void Init(const Camera& camera);
	void Update(Camera& camera, const XMFLOAT3& enemyPos);
	void Draw(const PipeClass::PipelineSet& pipelineSet);
	void Finalize();

public:
	XMFLOAT3 GetPos() { return pos; }				//ƒ|ƒWƒVƒ‡ƒ“‚ð•Ô‚·
	XMFLOAT3 GetDirection() { return direction; }	//Œü‚¢‚Ä‚é•ûŒü‚ð•Ô‚·
	bool GetAttackFlag() { return attackFlag; }		//UŒ‚‚µ‚Ä‚¢‚é‚©‚ð•Ô‚·
	bool GetStopTimeFlag() { return stopTimeFlag; }	//ŽžŠÔ‚ðŽ~‚ß‚Ä‚¢‚é‚©Ž~‚ß‚Ä‚¢‚È‚¢‚©‚ð•Ô‚·
	float GetSpeed() { return MOVE_SPEED; }
	void Damaged() { if (hp > 0) hp--; }			//HP‚ðŒ¸‚ç‚·

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

