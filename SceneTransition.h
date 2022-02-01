#pragma once
#include "TextureMgr.h"
#include "Sprite.h"
#include <DirectXMath.h>

using namespace DirectX;

class SceneTransition
{
private:
	enum NowStatus
	{
		before,
		now,
		after
	};

private:
	const float STICK_SIZE_X = 1520;
	const float STICK_SIZE_Y = 360;
	const float GEAR_SIZE_X = 480;
	const float GEAR_SIZE_Y = 480;

	const float MOVE_SPEED = 30;
	const float ROTATE_SPEED = 5;

	int BACK_STICK;
	int SCENE_GEAR_DOWN;
	int SCENE_GEAR_UP;
	int nowStatus;				//‘JˆÚ
	float easeTimer;
	bool isTransitionMiddle;	//‘JˆÚ‚Ìƒ^ƒCƒ~ƒ“ƒO(’š“x‰æ–Ê‚ª‰B‚ê‚½Žž)

	Sprite right_back_stick;
	Sprite left_back_stick;
	Sprite scene_gear_down;
	Sprite scene_gear_up;

public:
	SceneTransition();
	~SceneTransition();
	void Init();
	void Update();
	void Draw();
	void Finalize();

	void On() { nowStatus = now; }
	int GetNowStatus() { return nowStatus; }
	bool Change()
	{
		if (isTransitionMiddle)
		{
			isTransitionMiddle = false;
			return true;
		}
		else { return false; }
	}
};

