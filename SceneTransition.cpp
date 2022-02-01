#include "SceneTransition.h"

SceneTransition::SceneTransition()
{
	easeTimer = 0.0f;
	nowStatus = before;
	isTransitionMiddle = false;

	//BACK_STICK = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/BACK_STICK.png");
	BACK_STICK = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/stick.png");
	SCENE_GEAR_DOWN = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/SCENE_GEAR_DOWN.png");
	SCENE_GEAR_UP = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/SCENE_GEAR_UP.png");

	right_back_stick.Init(BACK_STICK);
	left_back_stick.Init(BACK_STICK);
	scene_gear_down.Init(SCENE_GEAR_DOWN);
	scene_gear_up.Init(SCENE_GEAR_UP);

	right_back_stick.anchorpoint = { 0,0 };
	left_back_stick.anchorpoint = { 0,0 };

	right_back_stick.size = { STICK_SIZE_X, STICK_SIZE_Y };
	left_back_stick.size = { STICK_SIZE_X, STICK_SIZE_Y };
	scene_gear_down.size = { GEAR_SIZE_X, GEAR_SIZE_Y };
	scene_gear_up.size = { GEAR_SIZE_X, GEAR_SIZE_Y };

	//棒
	right_back_stick.position = {		//右下(左上基準)
		STICK_SIZE_X + 880 + 120,		//X
		540,							//Y
		0 };
	left_back_stick.position = {		//左上(左上基準)
		-STICK_SIZE_X + 400 - 120,		//X
		180,							//Y
		0 };

	//ギア
	scene_gear_down.position = {		//右下(中心基準)
		1280 + 120 + GEAR_SIZE_X / 2,	//X
		540,							//Y
		0 };
	scene_gear_up.position = {			//左上(中心基準)
		-GEAR_SIZE_X / 2 - 120,			//X
		180,							//Y
		0 };
}

SceneTransition::~SceneTransition()
{
}

void SceneTransition::Init()
{
	easeTimer = 0.0f;
	nowStatus = before;
	isTransitionMiddle = false;

	//棒
	right_back_stick.position = {		//右下(左上基準)
		STICK_SIZE_X + 880 + 120,		//X
		540,							//Y
		0 };
	left_back_stick.position = {		//左上(左上基準)
		-STICK_SIZE_X + 400 - 120,		//X
		180,							//Y
		0 };

	//ギア
	scene_gear_down.position = {		//右下(中心基準)
		1280 + 120 + GEAR_SIZE_X / 2,	//X
		540,							//Y
		0 };
	scene_gear_up.position = {			//左上(中心基準)
		-GEAR_SIZE_X / 2 - 120,			//X
		180,							//Y
		0 };
}

void SceneTransition::Update()
{
	if (nowStatus == now)
	{
		//右下
		if (right_back_stick.position.x > -800)
		{
			right_back_stick.position.x -= MOVE_SPEED;

			scene_gear_down.position.x -= MOVE_SPEED;
			scene_gear_down.rotation -= ROTATE_SPEED;

			right_back_stick.SpriteUpdate();
			scene_gear_down.SpriteUpdate();
		}

		//左上
		if (left_back_stick.position.x < 2120)
		{
			left_back_stick.position.x += MOVE_SPEED;

			scene_gear_up.position.x += MOVE_SPEED;
			scene_gear_up.rotation += ROTATE_SPEED;

			left_back_stick.SpriteUpdate();
			scene_gear_up.SpriteUpdate();
		}

		//判定(真ん中)
		if (right_back_stick.position.x <= STICK_SIZE_X / 2 &&
			left_back_stick.position.x >= STICK_SIZE_X / 2)
		{
			isTransitionMiddle = true;
		}
		else { isTransitionMiddle = false; }

		//判定(動ききったら)
		if (right_back_stick.position.x <= -800 &&
			left_back_stick.position.x >= 2120)
		{
			Init();
			nowStatus = after;
		}
	}
}

void SceneTransition::Draw()
{
	if (nowStatus == now)
	{
		//右下
		if (right_back_stick.position.x >= -800)
		{
			right_back_stick.SpriteDraw();
		}

		//左上
		if (left_back_stick.position.x <= 2120)
		{
			left_back_stick.SpriteDraw();
		}

		scene_gear_down.SpriteDraw();
		scene_gear_up.SpriteDraw();
	}
}

void SceneTransition::Finalize()
{
}
