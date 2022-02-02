#include "Base/WinAPI.h"
#include "Base/MyDirectX.h"
#include "Input/DirectInput.h"
#include "2D/SpriteCommon.h"
#include "2D/Sprite.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <math.h>
#include <DirectXTex.h>
#include <wrl.h>
#include <xaudio2.h>
#include <fstream>
#include "2D/DebugText.h"
#include "3D/Object3DCommon.h"
#include "3D/Object3D.h"
#include "Camera.h"
#include "EaseClass.h"
#include "Sound.h"
#include "Base/GraphicsPipeline3D.h"
#include "IGraphicsPipeline.h"
#include "TextureMgr.h"
#include "Model.h"
#include "ModelPipeline.h"
#include "EnemyMgr.h"
#include "Wall.h"
#include "Player.h"
#include <vector>
#include "WallMgr.h"
#include "particleManager.h"
#include "Vector3.h"
#include "Shake.h"
#include "BombEffect.h"
#include "SceneTransition.h"
using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xaudio2.lib")

/*
	素敵なコメント
	aaaaaaaaaa
	頑張っていこう！
	こんにちは！
*/


const int window_width = 1280;
const int window_height = 720;

//Player初期位置(簡易)

//ステージ数
const int MAX_STAGE_NUM = 3;

enum Scenes
{
	TITLE,
	STAGESELECT,
	GAME,
	CLEAR,
	GAMEOVER,
	CHOISE_DEAD,
	CHOISE_CLEAR
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//WindowsAPI初期化処理
#pragma region WindowsAPI

	WinAPI* Win = WinAPI::GetInstance();

	Win->Init(window_width, window_height);
#pragma endregion
#pragma region sound(xAudioInstance)

	Sound::StaticInitialize();
	int EnterData = Sound::SoundLoadWave("Resources/sounds/SE_Enter.wav");
	int DamageData = Sound::SoundLoadWave("Resources/sounds/SE_Damage.wav");
	int BGMData = Sound::SoundLoadWave("Resources/sounds/BGM_Ingame.wav");
	int SelectData = Sound::SoundLoadWave("Resources/sounds/SE_Choice.wav");
	int BreakData = Sound::SoundLoadWave("Resources/sounds/SE_Break.wav");
	int OutBGMData = Sound::SoundLoadWave("Resources/sounds/BGM_OutGame.wav");
	int ClearData = Sound::SoundLoadWave("Resources/sounds/SE_Clear.wav");
	int GameoverData = Sound::SoundLoadWave("Resources/sounds/SE_Gameover.wav");
	Sound OutBgm(OutBGMData);
	Sound enterSE(EnterData);
	Sound BGM(BGMData);
	Sound SelectSE(SelectData);
	Sound EnemyDamageSE(BreakData);
	Sound ClearSE(ClearData);
	Sound GameoverSE(GameoverData);
#pragma endregion

	//DirectX初期化処理 ここまで
	MyDirectX* myDirectX = MyDirectX::GetInstance();

	IGraphicsPipeline* Pipe3D = GraphicsPipeline3D::GetInstance();
	IGraphicsPipeline* model3D = ModelPipeline::GetInstance();

#pragma region DirectInput
	Input* input = Input::GetInstance();
	input->Init(Win->w, Win->hwnd);
#pragma endregion



	Camera cam;

#pragma region particles
	ParticleManager::StaticInitialize(&cam);
#pragma endregion

#pragma region Init
	const float wallHeight = 50;
	ParticleManager part;

	cam.Init(Vector3(0, 250, 0), Vector3(0, 0, 0), { 0,0,0 }, { 0,0,1 });
	float angle = 0.0f;	//カメラの回転角

	DebugText debugText;
	//モデル生成
	Model boxModel;
	Model domeModel;
	//モデルのファイル読み込み
	boxModel.CreateModel("box");
	domeModel.CreateModel("skydome");

	Object3D box;

	box.scale = { 10.0f, 10.0f, 10.0f };
	box.position = { 20,10,0 };
	box.Init(cam);

	Object3D dome;
	dome.Init(cam);
	dome.scale = { 3.0f, 3.0f, 3.0f };
	dome.position = { 10, 0,0 };
	bool isTexture = false;

	//仮
	Sphere sphere;
	sphere.center = { 20,10,0 };
	sphere.radius = 20.0f;

	Wall::SetModel(boxModel);
	int deadGraph = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/DEAD_DEAD.png");
	int clearGraph = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/CLEAR_CLEAR.png");

	Sprite deadSprite;
	Sprite clearSprite;
	deadSprite.Init(deadGraph, { 0, 0 });
	clearSprite.Init(clearGraph, { 0, 0 });

	int particleGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/effect1.png");

	SceneTransition sceneTransition;
	//sceneTransition.Init();

	Vector3 up(0, 0, 1), left(-1, 0, 0);

	Vector3 down = -up;
	Vector3 right = -left;

#pragma region startPos

	std::vector<XMFLOAT3> StartPositions;
	const XMFLOAT3 STAGE_1 = { -450.1f,0,0 };
	const XMFLOAT3 STAGE_2 = { -450.1f,0,-125.1f };
	const XMFLOAT3 STAGE_3 = { -450.1f,0,0 };
	const XMFLOAT3 STAGE_4 = { 0,0,0 };

	StartPositions.push_back(STAGE_1);
	StartPositions.push_back(STAGE_2);
	StartPositions.push_back(STAGE_3);
	StartPositions.push_back(STAGE_4);

#pragma endregion
#pragma region goal
	//floor.scale = { 1000.0f, 2.0f, 300.0f };
	XMFLOAT3 lowerLeft[3];
	XMFLOAT3 upperRight[3];
	lowerLeft[0] = { 350,0,-40 };
	upperRight[0] = { 450,0,40 };

	lowerLeft[1] = { 400,0, 100 };
	upperRight[1] = { 450,0,150 };

	lowerLeft[2] = { 400,0, -150 };
	upperRight[2] = { 500,0,-100 };

#pragma endregion





	deadSprite.size = deadSprite.texSize;

	clearSprite.size = clearSprite.texSize;

	deadSprite.SpriteUpdate();
	clearSprite.SpriteUpdate();
	Player player(deadGraph, clearGraph, particleGH, DamageData);
	player.Init(cam, StartPositions[0]);
	bool isdead;
	int hp;


	Object3D floor;
	floor.scale = { 1000.0f, 2.0f, 300.0f };
	floor.position = { 0.0f, -20.0f, 0.0f };
	floor.color = { 3.0f,2.0f ,2.0f ,1.0f };
	floor.Init(cam);


	DirectX::XMFLOAT3 pillarScale = { 10, wallHeight, 10 };
	DirectX::XMFLOAT3 LPillarScale = { 50, wallHeight, 50 };
	DirectX::XMFLOAT3 wideWallScale = { 200, wallHeight, 10 };
	DirectX::XMFLOAT3 heightWallScale = { 10, wallHeight, 100 };
	DirectX::XMFLOAT3 LHeightWallScale = { 80, wallHeight, 150 };
	DirectX::XMFLOAT3 LWidthWallScale = { 150, wallHeight, 100 };

#pragma endregion

	std::vector<Wall> outWall;
	outWall.resize(4);
	outWall[0].Init(cam, { -(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 1000, floor.scale.z });
	outWall[1].Init(cam, { 0.0f,floor.position.y ,-(floor.scale.z / 2) }, { floor.scale.x, 1000, 10 });
	outWall[2].Init(cam, { +(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 1000, floor.scale.z });
	outWall[3].Init(cam, { 0.0f,floor.position.y ,+(floor.scale.z / 2) }, { floor.scale.x, 1000, 10 });
#pragma region １面壁

	std::vector<Wall> loomWalls;
	Wall wall[14];



	wall[0].Init(cam, { 0.0f,floor.position.y ,0.0f }, pillarScale);
	loomWalls.push_back(wall[0]);

	//入口奥
	wall[1].Init(cam, { -floor.scale.x / 2 + 100,floor.position.y ,floor.scale.z / 2 - 100 }, wideWallScale);
	loomWalls.push_back(wall[1]);
	wall[2].Init(cam, { -floor.scale.x / 2 + 200,floor.position.y ,floor.scale.z / 2 - 50 }, heightWallScale);
	loomWalls.push_back(wall[2]);

	//入口手前
	wall[3].Init(cam, { -floor.scale.x / 2 + 100,floor.position.y ,-floor.scale.z / 2 + 100 }, wideWallScale);
	loomWalls.push_back(wall[3]);
	wall[4].Init(cam, { -floor.scale.x / 2 + 200,floor.position.y ,-floor.scale.z / 2 + 50 }, heightWallScale);
	loomWalls.push_back(wall[4]);

	//入り口側中央柱
	wall[5].Init(cam, { -180,floor.position.y ,0 }, LHeightWallScale);
	loomWalls.push_back(wall[5]);

	//出口側二本柱
	wall[6].Init(cam, { +200,floor.position.y ,-80 }, LPillarScale);
	loomWalls.push_back(wall[6]);
	wall[7].Init(cam, { +200,floor.position.y ,+80 }, LPillarScale);
	loomWalls.push_back(wall[7]);

	//中間通路壁
	wall[8].Init(cam, { 0,floor.position.y ,floor.scale.z / 2 - 50 }, LWidthWallScale);
	loomWalls.push_back(wall[8]);
	wall[9].Init(cam, { 0,floor.position.y ,-floor.scale.z / 2 + 50 }, LWidthWallScale);
	loomWalls.push_back(wall[9]);

	//出口奥
	wall[10].Init(cam, { floor.scale.x / 2 - 100,floor.position.y ,floor.scale.z / 2 - 100 }, wideWallScale);
	loomWalls.push_back(wall[10]);
	wall[11].Init(cam, { floor.scale.x / 2 - 200,floor.position.y ,floor.scale.z / 2 - 50 }, heightWallScale);
	loomWalls.push_back(wall[11]);

	//出口手前
	wall[12].Init(cam, { +floor.scale.x / 2 - 100,floor.position.y ,-floor.scale.z / 2 + 100 }, wideWallScale);
	loomWalls.push_back(wall[12]);
	wall[13].Init(cam, { +floor.scale.x / 2 - 200,floor.position.y ,-floor.scale.z / 2 + 50 }, heightWallScale);
	loomWalls.push_back(wall[13]);

	for (int i = 0; i < outWall.size(); i++)
	{
		loomWalls.push_back(outWall[i]);
	}
	WallMgr::Instance()->Init(loomWalls);

#pragma endregion

#pragma region 二面壁

	std::vector<Wall> townWalls;
	Wall townWallData[12];

	//初期地点右上
	townWallData[0].Init(cam, { -floor.scale.x / 2 + 75,floor.position.y, -75.0f }, { 50, wallHeight, 50 });
	townWalls.push_back(townWallData[0]);

	//左壁沿い
	townWallData[1].Init(cam, { -floor.scale.x / 2 + 25,floor.position.y ,25.0f }, { 50, wallHeight, 50 });
	townWalls.push_back(townWallData[1]);

	//初期地点右右上
	townWallData[2].Init(cam, { -floor.scale.x / 2 + 250,floor.position.y, -75.0f }, { 50, wallHeight, 50 });
	townWalls.push_back(townWallData[2]);

	//初期地点側上部縦長壁
	townWallData[3].Init(cam, { -floor.scale.x / 2 + 180,floor.position.y, 40.0f }, { 50, wallHeight, 100 });
	townWalls.push_back(townWallData[3]);

	//中央上部左横長壁
	townWallData[4].Init(cam, { -140,floor.position.y ,floor.scale.z / 2 - 80 }, { 150, wallHeight, 75 });
	townWalls.push_back(townWallData[4]);

	//中央下部大壁
	townWallData[5].Init(cam, { -20,floor.position.y ,-floor.scale.z / 2 + 50 }, { 140, wallHeight, 100 });
	townWalls.push_back(townWallData[5]);

	//中央上部右大壁
	townWallData[6].Init(cam, { +100,floor.position.y ,60.0f }, { 150, wallHeight, 120 });
	townWalls.push_back(townWallData[6]);

	//右側下段壁
	townWallData[7].Init(cam, { +200,floor.position.y ,-80 }, { 150, wallHeight, 50 });
	townWalls.push_back(townWallData[7]);

	//右上隔離壁縦
	townWallData[8].Init(cam, { floor.scale.x / 2 - 230,floor.position.y ,floor.scale.z / 2 - 50 }, { 50, wallHeight, 100 });
	townWalls.push_back(townWallData[8]);
	//右下端壁
	townWallData[9].Init(cam, { floor.scale.x / 2 - 75,floor.position.y ,-floor.scale.z / 2 + 100 }, { 150, wallHeight, 200 });
	townWalls.push_back(townWallData[9]);

	//隔離部屋内壁
	townWallData[10].Init(cam, { floor.scale.x / 2 - 75,floor.position.y ,floor.scale.z / 2 - 100 }, { 150, wallHeight, 100 });
	townWalls.push_back(townWallData[10]);

	//壁抜け対策
	townWallData[11].Init(cam, { -floor.scale.x / 2 ,floor.position.y ,-floor.scale.z / 2 }, { 40, wallHeight, 40 });
	townWalls.push_back(townWallData[11]);

	for (int i = 0; i < outWall.size(); i++)
	{
		townWalls.push_back(outWall[i]);
	}

	WallMgr::Instance()->Init(townWalls);
	//wall


	EnemyMgr::Instance()->Init(cam);

	Scenes nowScene = TITLE;
	Scenes oldScene = nowScene;
	std::vector<XMFLOAT3> loomEnemyGeneratePos;
	loomEnemyGeneratePos.push_back(XMFLOAT3{ -255, 0, 105 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ -255, 0, -105 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ -85, 0, 0 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ 130, 0, -50 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ 130, 0, 50 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ 260, 0, 0 });
	std::vector<XMFLOAT3> enemyForwardVec;
	enemyForwardVec.push_back(XMFLOAT3(0, 0, -1));
	enemyForwardVec.push_back(XMFLOAT3(0, 0, 1));
	enemyForwardVec.push_back(XMFLOAT3(1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(-1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(-1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(1, 0, 0));

	std::vector<XMFLOAT3> townEnemyGeneratePos;
	townEnemyGeneratePos.push_back(XMFLOAT3{ -336, 0, -70 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ -396, 0, 64 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ -148, 0, -26 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ -23, 0, 66 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ 88, 0, -80 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ 304, 0, 0 });




#pragma endregion

#pragma region 3面壁
	std::vector<Wall> thirdStageWalls;
	Wall thirdStageWallData[9];

	//入口壁
	thirdStageWallData[0].Init(cam, Vector3(-floor.scale.x / 2 + 50, -20, floor.scale.z / 2 - 50), Vector3(100, wallHeight, 100));
	thirdStageWalls.push_back(thirdStageWallData[0]);
	thirdStageWallData[1].Init(cam, Vector3(-floor.scale.x / 2 + 50, -20, -floor.scale.z / 2 + 50), Vector3(100, wallHeight, 100));
	thirdStageWalls.push_back(thirdStageWallData[1]);

	//鍵壁
	thirdStageWallData[2].Init(cam, Vector3(-floor.scale.x / 2 + 150, -20, -60), Vector3(150, wallHeight, 20));
	thirdStageWalls.push_back(thirdStageWallData[2]);
	thirdStageWallData[3].Init(cam, Vector3(-floor.scale.x / 2 + 215, -20, 0), Vector3(20, wallHeight, 100));
	thirdStageWalls.push_back(thirdStageWallData[3]);

	//中央縦壁
	thirdStageWallData[4].Init(cam, Vector3(-floor.scale.x / 2 + 350, -20, 40), Vector3(40, wallHeight, 220));
	thirdStageWalls.push_back(thirdStageWallData[4]);

	//中央でっぱり
	thirdStageWallData[5].Init(cam, Vector3(-floor.scale.x / 2 + 410, -20, +60), Vector3(80, wallHeight, 20));
	thirdStageWalls.push_back(thirdStageWallData[5]);

	//ゴール側竪壁
	thirdStageWallData[6].Init(cam, Vector3(-floor.scale.x / 2 + 700, -20, +60), Vector3(300, wallHeight, 20));
	thirdStageWalls.push_back(thirdStageWallData[6]);


	//右上横壁
	thirdStageWallData[7].Init(cam, Vector3(-floor.scale.x / 2 + 650, -20, -55), Vector3(40, wallHeight, 220));
	thirdStageWalls.push_back(thirdStageWallData[7]);

	//ゴール壁
	thirdStageWallData[8].Init(cam, Vector3(floor.scale.x / 2 - 80, -20, -100), Vector3(200, wallHeight, 20));
	thirdStageWalls.push_back(thirdStageWallData[8]);
	for (int i = 0; i < outWall.size(); i++)
	{
		thirdStageWalls.push_back(outWall[i]);
	}

	std::vector<XMFLOAT3> thirdStageEnemyGeneratePos;
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -346, 0, 97 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -222, 0, 97 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -222, 0, -7 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -365, 0, -106 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 2, 0, -100 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -89, 0, 10 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -89, 0, -55 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 87, 0, 10 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 87, 0, -55 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ -89, 0, 108 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 87, 0, 108 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 228, 0, 108 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 430, 0, 107 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 430, 0, -47 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 342, 0, -47 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 211, 0, 16 });
	thirdStageEnemyGeneratePos.push_back(XMFLOAT3{ 221, 0, -102 });
	std::vector<XMFLOAT3> thirdStageEnemyGenerateAngle;
	thirdStageEnemyGenerateAngle.resize(thirdStageEnemyGeneratePos.size());
	for (int i = 0; i < thirdStageEnemyGenerateAngle.size(); i++)
	{
		thirdStageEnemyGenerateAngle[i] = up;
	}

#pragma endregion

	//EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, cam);

#pragma region 画像初期化

	/*画像初期化*/
	//再生ボタンみたいな
	int startGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/start.png");
	//時間停止ボタンみたいな
	int stopGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/stop.png");
	//Playerの攻撃範囲
	int haniGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/hani.png");
	BombEffect::SetTexture(particleGH);

	Sprite spriteStart;
	spriteStart.Init(startGH);

	Sprite spriteStop;
	spriteStop.Init(stopGH);

	Sprite hani;
	hani.Init(haniGH, XMFLOAT2(0.5f, 1.0f));

	spriteStart.position = { window_width / 2,window_height / 2,0 };
	spriteStop.position = { window_width / 2,window_height / 2,0 };
	hani.position = { window_width / 2,window_height / 2,0 };

	bool stopDraw = false;
	bool startDraw = false;

	spriteStart.color.w = 0.0f;
	spriteStop.color.w = 0.0f;
	hani.color.w = 0.5f;

	//敵にダメージ入ったかどうか
	bool damaged = false;

	//クリアしてるかどうか
	bool isClear = false;
	bool isDead = false;

	//タイトル画面
	Sprite titleLogo;
	int titleTex = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/TitleGraph_Bright.png");
	titleLogo.Init(titleTex);
	XMFLOAT2 titleTexSize = titleLogo.texSize;
	titleLogo.position = { window_width / 2, window_height / 2 , 0.0f };
	titleLogo.size = { window_width, window_height };
	titleLogo.SpriteUpdate();

	//タイトルのボタン表示
	int TITLE_KEYS = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/TITLE_KEYBOARD.png");
	int TITLE_PAD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/TITLE_CONTROLLER.png");

	Sprite title_keys;
	Sprite title_pad;

	title_keys.Init(TITLE_KEYS);
	title_pad.Init(TITLE_PAD);

	title_keys.position = { window_width / 2,540,0 };
	title_pad.position = { window_width / 2,540,0 };

	title_keys.size = { 640,64 };
	title_pad.size = { 640,64 };

	//float alpha = 0.0f;

	bool isAdd = false;

	/*----------DEAD_CLEAR(CLEARとDEADはPlayerで管理済)----------*/
	int CLEAR_CHOICE = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/CLEAR_CHOICE.png");
	int CLEAR_FRAME_DOWN = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/CLEAR_FRAME_DOWN.png");
	int CLEAR_FRAME_UP = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/CLEAR_FRAME_UP.png");
	int DEAD_CHOICE = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/DEAD_CHOICE.png");
	int DEAD_FRAME_DOWN = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/DEAD_FRAME_DOWN.png");
	int DEAD_FRAME_UP = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/DEAD_CLEAR/DEAD_FRAME_UP.png");

	Sprite clear_choise;
	Sprite clear_frame_down;
	Sprite clear_frame_up;
	Sprite dead_choise;
	Sprite dead_frame_down;
	Sprite dead_frame_up;
	clear_choise.Init(CLEAR_CHOICE, { 0.0f, 0.0f });
	clear_frame_down.Init(CLEAR_FRAME_DOWN, { 0.0f, 0.0f });
	clear_frame_up.Init(CLEAR_FRAME_UP, { 0.0f, 0.0f });
	dead_choise.Init(DEAD_CHOICE, { 0.0f, 0.0f });
	dead_frame_down.Init(DEAD_FRAME_DOWN, { 0.0f, 0.0f });
	dead_frame_up.Init(DEAD_FRAME_UP, { 0.0f, 0.0f });

	clear_choise.size = { window_width, window_height };
	clear_choise.SpriteUpdate();
	clear_frame_down.size = { window_width, window_height };
	clear_frame_down.SpriteUpdate();
	clear_frame_up.size = { window_width, window_height };
	clear_frame_up.SpriteUpdate();
	dead_choise.size = { window_width, window_height };
	dead_choise.SpriteUpdate();
	dead_frame_down.size = { window_width, window_height };
	dead_frame_down.SpriteUpdate();
	dead_frame_up.size = { window_width, window_height };
	dead_frame_up.SpriteUpdate();

	/*----------HUD----------*/
	int HUD_CONTROLL_PAD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_BASE_PAD.png");
	int HUD_CONTROLL_KEYS = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_BASE_KEYS.png");

	int HUD_LIFE_BASE = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_LifeBase.png");
	int HUD_TIMESTATE = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_TimeState.png");
	int HUD_LIFE = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_LIFE.png");

	int HUD_PLAY = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_PlayText.png");
	int HUD_STOP = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_StopText.png");

	Sprite hud_base_pad;
	Sprite hud_base_keys;

	Sprite hud_base_life;
	Sprite hud_timestate;

	Sprite hud_life_1;
	Sprite hud_life_2;
	Sprite hud_life_3;
	Sprite hud_play;
	Sprite hud_stop;

	hud_base_pad.Init(HUD_CONTROLL_PAD, { 0, 0 });
	hud_base_keys.Init(HUD_CONTROLL_KEYS, { 0, 0 });

	hud_base_life.Init(HUD_LIFE_BASE, { 0, 1 });
	hud_life_1.Init(HUD_LIFE, { 0, 1 });
	hud_life_2.Init(HUD_LIFE, { 0, 1 });
	hud_life_3.Init(HUD_LIFE, { 0, 1 });

	hud_timestate.Init(HUD_TIMESTATE, { 1, 0 });
	hud_play.Init(HUD_PLAY, { 1, 0 });
	hud_stop.Init(HUD_STOP, { 1, 0 });


	hud_base_pad.size = hud_base_pad.texSize;
	hud_base_keys.size = hud_base_keys.texSize;

	hud_base_life.size = hud_base_life.texSize;
	hud_timestate.size = hud_timestate.texSize;
	hud_life_1.size = hud_life_1.texSize;
	hud_life_2.size = hud_life_2.texSize;
	hud_life_3.size = hud_life_3.texSize;
	hud_play.size = hud_play.texSize;
	hud_stop.size = hud_stop.texSize;

	hud_base_pad.position = XMFLOAT3(32, 32, 0);
	hud_base_keys.position = XMFLOAT3(32, 32, 0);

	hud_base_life.position = XMFLOAT3(32, window_height - 32, 0);
	hud_life_1.position = XMFLOAT3(32, window_height - 32, 0);
	hud_life_2.position = XMFLOAT3(32 + (64 * 1), window_height - 32, 0);
	hud_life_3.position = XMFLOAT3(32 + (64 * 2), window_height - 32, 0);
	hud_timestate.position = XMFLOAT3(window_width - 32, 32, 0);
	hud_play.position = XMFLOAT3(window_width - 32, 32, 0);
	hud_stop.position = XMFLOAT3(window_width - 32, 0, 0);
	hud_base_pad.SpriteUpdate();
	hud_base_keys.SpriteUpdate();


	hud_timestate.SpriteUpdate();
	/*hud_play.SpriteUpdate();
	hud_stop.SpriteUpdate();*/

	hud_base_life.SpriteUpdate();

	//合体させたHUD
	int HUD_PLAY_STOP = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_PLAY_STOP.png");
	Sprite hud_play_stop;
	hud_play_stop.Init(HUD_PLAY_STOP, XMFLOAT2(1.0, 0.0));
	hud_play_stop.size = { 192,80 };
	hud_play_stop.position = XMFLOAT3(1248, 32, 0);
	float hud_ease = 0.0f;

	/*----------SCENE_CHANGE----------*/
	int BACK_STICK = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/BACK_STICK.png");
	int SCENE_GEAR_DOWN = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/SCENE_GEAR_DOWN.png");
	int SCENE_GEAR_UP = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/SCENE_CHANGE/SCENE_GEAR_UP.png");

	Sprite back_stick;
	Sprite scene_gear_down;
	Sprite scene_gear_up;

	back_stick.Init(BACK_STICK);
	scene_gear_down.Init(SCENE_GEAR_DOWN);
	scene_gear_up.Init(SCENE_GEAR_UP);

	back_stick.size = { window_width, window_height };
	scene_gear_down.size = { window_width, window_height };
	scene_gear_up.size = { window_width, window_height };

	/*-------STAGE_SELECT-------*/
	int STAGE_SELECT_GRAPH_PAD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/SELECT_CONTROLLER.png");
	int STAGE_SELECT_GRAPH_KEYBOARD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/SELECT_KEYBOARD.png");

	Sprite stage_select_pad;
	Sprite stage_select_keys;
	stage_select_pad.Init(STAGE_SELECT_GRAPH_PAD, { 0.0f, 0.0f });
	stage_select_keys.Init(STAGE_SELECT_GRAPH_KEYBOARD, { 0.0f, 0.0f });

	stage_select_pad.size = { window_width, window_height };
	stage_select_keys.size = { window_width, window_height };
	stage_select_pad.SpriteUpdate();
	stage_select_keys.SpriteUpdate();

	int MASK = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/mask_circle.png");
	Sprite mask;
	mask.Init(MASK);
	mask.size = { 0,0 };
	mask.position = { window_width / 2,window_height / 2,0 };
	mask.color.w = 0.1f;

	float easeTimer = 0.0f;

	//開始時の文字
	int GO = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/go.png");
	Sprite go;
	go.Init(GO, XMFLOAT2(0.0f, 0.0f));
	go.size = { 640,180 };
	go.position = { 320.0f,-180.0f,0 };

	float easeTimer_START = 1.0f;
	bool UpdateStart = false;

#pragma endregion

#pragma region 敵

	int stageNum = 0;
	bool choiseNum = false;



	std::vector<XMFLOAT3> loomEnemyAngles;
	loomEnemyAngles.push_back(down);
	loomEnemyAngles.push_back(down);
	loomEnemyAngles.push_back(left);
	loomEnemyAngles.push_back(left);
	loomEnemyAngles.push_back(up);
	loomEnemyAngles.push_back(left);



	std::vector<XMFLOAT3> townEnemyAngles;
	townEnemyAngles.push_back(down);
	townEnemyAngles.push_back(down);
	townEnemyAngles.push_back(left);
	townEnemyAngles.push_back(left);
	townEnemyAngles.push_back(up);
	townEnemyAngles.push_back(left);

#pragma endregion

	BombEffect bomb;

	bool StickFlag = false;
	bool StickOldFlag = false;
	bool resultFlag = false;
	int resultSelect = 0;

	//トリガー判定用
	bool isTrigger = false;
	//HP管理用
	bool hp_1to0 = false;
	bool hp_2to1 = false;
	bool hp_3to2 = false;

	//タイトル用
	Object3D* myObj = player.GetObj();
	Model* myModel = player.GetModel();

	Object3D* enemyObj = EnemyMgr::Instance()->GetObj();
	Model* enemyModel = EnemyMgr::Instance()->GetModel();

	myObj->position = { 0,0,0 };
	myObj->rotation = { 0,135,0 };

	enemyObj->position = { 0,0,0 };
	enemyObj->rotation = myObj->rotation;

	cam.target = { 0,0,0 };
	cam.eye = { 0,30,-80 };

	float larpTimer = 0.0f;
	float larpTimer2 = 0.2f;

	OutBgm.PlayLoop();
	//if (FAILED(result))
	//{
	//	return result;
	//}
	//描画初期化処理 ここまで
	while (Win->loopBreak()) //ゲームループ
	{
		//ウィンドウメッセージ処理
#pragma region WindowMessage
		Win->msgCheck();
#pragma endregion
		// DirectX毎フレーム処理 ここから
		input->Update();

		oldScene = nowScene;

#pragma region 画像処理

		//画像処理
		int stopDelay = player.GetStopTimeDelay(); //CT 0~Delay
		int stopCount = player.GetStopTimeCount(); //時間停止 0~60
		bool isStop = player.GetStopTimeFlag(); //停止中か否か

		if (!stopDraw && !isStop &&
			(input->KeyTrigger(DIK_RETURN) || input->ButtonTrigger(XINPUT_GAMEPAD_A)) &&
			stopDelay == STOP_TIME_DELAY)
		{
			spriteStop.color.w = 0.7f;
			spriteStop.size = { 100.0f,100.0f };
			stopDraw = true;
		}

		if (!startDraw && isStop && stopCount >= STOP_TIME_COUNT - 1)
		{
			spriteStart.color.w = 0.7f;
			spriteStart.size = { 100.0f,100.0f };
			startDraw = true;
		}

		if (stopDraw && stopCount < 20) {
			spriteStop.color.w -= stopCount / 100.0f;
			spriteStop.size.x += stopCount;
			spriteStop.size.y += stopCount;
		}
		else { stopDraw = false; }

		if (startDraw && stopDelay < 20) {
			spriteStart.color.w -= stopDelay / 100.0f;
			spriteStart.size.x += stopDelay;
			spriteStart.size.y += stopDelay;
		}
		else { startDraw = false; }

		//範囲の画像
		hani.anchorpoint = { 0.5,0.5 };
		hani.rotation = player.GetAngle() + 90.0f;

		XMFLOAT3 vec3 = player.GetVec3();
		float width = window_width;
		float height = window_height;

		XMFLOAT3 CtoP = player.GetCameraToPlayer();
		hani.position = { width / 2 + (CtoP.x * 2) ,height / 2 - (CtoP.z * 2),0 };
		if (!player.IsDead()) { spriteStart.SpriteUpdate(); }
		if (!player.IsDead()) { spriteStop.SpriteUpdate(); }
		if (!player.IsDead()) { hani.SpriteUpdate(); }

		if (input->ButtonTrigger(XINPUT_GAMEPAD_X))
		{
			int a = 0;
		}

		if (isStop)
		{
			if (easeTimer < 0.0f) { easeTimer = 0.0f; }
			if (easeTimer < 1.0f) { easeTimer += 0.02f; }
			mask.size.x = (1920 - 0) * player.easeOutCubic(easeTimer) + 0;
			mask.size.y = (1920 - 0) * player.easeOutCubic(easeTimer) + 0;
		}
		else
		{
			if (easeTimer > 1.0f) { easeTimer = 1.0f; }
			if (easeTimer > 0.0f) { easeTimer -= 0.02f; }
			mask.size.x = (0 - 1920) * player.easeOutCubic(1 - easeTimer) + 1920;
			mask.size.y = (0 - 1920) * player.easeOutCubic(1 - easeTimer) + 1920;
		}
		mask.SpriteUpdate();

		//開始時文字
		if (easeTimer_START < 1.0f) {
			easeTimer_START += 0.006f;
			if (easeTimer_START >= 0.0f) { go.position.y = (900 - (-180)) * player.easeInOutSine(easeTimer_START) + (-180); }
		}
		if (easeTimer_START >= 0.7f) { UpdateStart = true; }
		go.SpriteUpdate();


#pragma endregion


		StickOldFlag = StickFlag;
		if (fabs(input->LStick().x) > 0.0f)
		{
			StickFlag = true;
		}
		else
		{
			StickFlag = false;
		}

		bool stickTrigger = (StickFlag && !StickOldFlag);
		//更新処理
		cam.Update();

		//遷移Update
		sceneTransition.Update();


		switch (nowScene)
		{

#pragma region TITLE_UPDATE

		case TITLE:
			titleLogo.SpriteTransferVertexBuffer();

			//文字
			if (input->isPadConnect())
			{
				if (title_pad.color.w >= 1.5f) { isAdd = false; }
				else if (title_pad.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { title_pad.color.w -= 0.02f; }
				else { title_pad.color.w += 0.02f; }

				title_pad.SpriteUpdate();
			}
			else
			{
				if (title_keys.color.w >= 1.5f) { isAdd = false; }
				else if (title_keys.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { title_keys.color.w -= 0.02f; }
				else { title_keys.color.w += 0.02f; }

				title_keys.SpriteUpdate();
			}

			if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A))
			{
				sceneTransition.On();

				enterSE.Play();
				//EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, cam);
			}
			if (sceneTransition.Change())
			{
				nowScene = STAGESELECT;
			}

			if (larpTimer < 1.0f) { larpTimer += 0.003f; }
			else { larpTimer = 0.0f; }
			if (larpTimer2 < 1.0f) { larpTimer2 += 0.003f; }
			else { larpTimer2 = 0.0f; }

			myObj->position.x = (200 - (-600)) * larpTimer + (-600);
			myObj->position.z = (-200 - 500) * larpTimer + 500;

			enemyObj->position.x = (200 - (-600)) * larpTimer2 + (-600);
			enemyObj->position.z = (-200 - 500) * larpTimer2 + 500;

			myObj->Update(cam);
			enemyObj->Update(cam);

			break;

#pragma endregion

#pragma region STAGESELECT_UPDATE

		case STAGESELECT:


			if (input->ButtonTrigger(XINPUT_GAMEPAD_B) || input->KeyTrigger(DIK_ESCAPE))
			{
				nowScene = TITLE;
				enterSE.Play();
			}
			if ((input->KeyTrigger(DIK_A) || input->KeyTrigger(DIK_D)) || (stickTrigger))
			{

				if (input->KeyTrigger(DIK_A) || input->LStick().x < 0.0f)
				{
					stageNum--;
				}
				if (input->KeyTrigger(DIK_D) || input->LStick().x > 0.0f)
				{
					stageNum++;
				}

				if (stageNum < 0)
				{
					stageNum = 0;
				}

				if (stageNum >= MAX_STAGE_NUM)
				{
					stageNum = MAX_STAGE_NUM - 1;
				}
				SelectSE.Play();
			}

			if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A))
			{
				if (!isTrigger)
				{
					sceneTransition.On();
					enterSE.Play();
					BGM.PlayLoop();
					OutBgm.Stop();
					isTrigger = true;
				}
			}

			if (sceneTransition.Change() && isTrigger)
			{
				easeTimer_START = 0.0f;
				if (UpdateStart) { UpdateStart = false; }
				isTrigger = false;
				nowScene = GAME;
				if (stageNum == 0)
				{
					player.Init(cam, StartPositions[stageNum]);
					cam.Init(XMFLOAT3(0, 250, 0), XMFLOAT3(0, 0, 0), StartPositions[stageNum], { 0,0,1 });
					WallMgr::Instance()->Init(loomWalls);
					EnemyMgr::Instance()->Init(cam);
					EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, loomEnemyAngles, cam);
				}
				else if (stageNum == 1)
				{
					player.Init(cam, StartPositions[stageNum]);
					cam.Init(XMFLOAT3(0, 250, 0), XMFLOAT3(0, 0, 0), StartPositions[stageNum], { 0,0,1 });
					WallMgr::Instance()->Init(townWalls);
					EnemyMgr::Instance()->Init(cam);
					EnemyMgr::Instance()->Generate(townEnemyGeneratePos, townEnemyAngles, cam);
				}
				else if (stageNum == 2)
				{
					player.Init(cam, StartPositions[stageNum]);
					cam.Init(XMFLOAT3(0, 250, 0), XMFLOAT3(0, 0, 0), StartPositions[stageNum], { 0,0,1 });
					WallMgr::Instance()->Init(thirdStageWalls);
					EnemyMgr::Instance()->Init(cam);
					EnemyMgr::Instance()->Generate(thirdStageEnemyGeneratePos, thirdStageEnemyGenerateAngle, cam);
				}

			}

			break;

#pragma endregion

#pragma region GAME_UPDATE

		case GAME:

			XMFLOAT3 moveSpeed = { input->LStick().x , 0.0f, input->LStick().y };

			box.Update(cam);
			XMFLOAT3 enemyPos = { 0,0,50 };
			isdead = player.IsDead();
			hp = player.GetHP();
			if (!isdead && !isClear) { player.Input(cam); }

			//フィールド上の壁
			XMFLOAT3 playerSpeed = player.GetVec3();

			for (int i = 0; i < 10; i++)
			{
				bool pushEnd;
				for (int i = 0; i < WallMgr::Instance()->GetWalls().size(); i++)
				{

					Vector3 push;
					push = WallMgr::Instance()->GetWalls()[i].PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);

					playerSpeed = { playerSpeed.x + push.x, playerSpeed.y + push.y ,playerSpeed.z + push.z };
					Vector3 ps;

					pushEnd = push.length() <= 0;
				}
				if (pushEnd)
				{
					player.SetVec3(playerSpeed);
					break;
				}
			}

			//if (input->KeyTrigger(DIK_RETURN))
			//{
			//	int a = 0;
			//}

			//敵と自機の押し戻し
			if (!isdead && !isClear) { player.PushBack(EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos())); }
			//自機更新処理
			player.Update(cam, EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos()));

			bomb.Update();

			box.position = enemyPos;
			box.Update(cam);

			box.position = { box.position.x + moveSpeed.x,box.position.y + moveSpeed.y ,box.position.z + moveSpeed.z };

			dome.Update(cam);
			floor.Update(cam);
			{
				Sphere pSphere;
				pSphere.center = XMLoadFloat3(&player.GetPos());
				pSphere.radius = 16;
				EnemyMgr::Instance()->Update(player.GetPos(), player.GetAngle(), player.GetStopTimeFlag(), player.GetAttackFlag());
				EnemyMgr::Instance()->UpdateData(cam);

				for (int i = 0; i < EnemyMgr::Instance()->MAX_ENEMY_COUNT; i++) {
					if (EnemyMgr::Instance()->CheckEnemyAttackToPlayer(i))
					{
						if (!damaged)
						{
							player.Damaged();
							hp = player.GetHP();
							//サイズ調整用
							if (hp == 2) { hp_3to2 = true; }
							if (hp == 1) { hp_2to1 = true; }
							if (hp == 0) { hp_1to0 = true; }
						}
						damaged = true;
					}
				}
				damaged = false;
			}

			//サイズ調整処理
			if (hp_3to2)
			{
				if (hud_life_3.color.w > 0.0f) { hud_life_3.color.w -= 0.03f; }
				else { hud_life_3.color.w = 0.0f;  hp_3to2 = false; }
			}
			if (hp_2to1)
			{
				if (hud_life_2.color.w > 0.0f) { hud_life_2.color.w -= 0.03f; }
				else { hud_life_2.color.w = 0.0f; hp_2to1 = false; }
			}
			if (hp_1to0)
			{
				if (hud_life_1.color.w > 0.0f) { hud_life_1.color.w -= 0.03f; }
				else { hud_life_1.color.w = 0.0f; hp_1to0 = false; }
			}


			//HP画像Update
			hud_life_1.SpriteUpdate();
			hud_life_2.SpriteUpdate();
			hud_life_3.SpriteUpdate();

			//合体させたHUD
			hud_play.SpriteUpdate();
			hud_stop.SpriteUpdate();
			hud_play_stop.SpriteUpdate();

			if (player.IsHit())
			{
				for (int i = 0; i < 50; i++)
				{
					float randX = (((float)rand() / RAND_MAX) * 2) - 1.0f;
					float randZ = (((float)rand() / RAND_MAX) * 2) - 1.0f;
					Vector3 tmp(randX, 0, randZ);
					float power = ((float)rand() / RAND_MAX) * 3;
					tmp = tmp.normalaize();
					part.Add(15, EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos()), tmp * power, XMFLOAT3(0, 0, 0), 10.0f, 0.0f);
				}
				EnemyMgr::Instance()->DeadNearEnemy();
				EnemyDamageSE.Play();
			}

			//自機死亡演出(シーン切り替えの直前に置く)
			player.DeathEffect(cam);

			//for (int i = 0; i < loomWalls.size(); i++)
			//{
			//	loomWalls[i].Update();
			//}
			WallMgr::Instance()->Update();
			for (int i = 0; i < outWall.size(); i++)
			{
				outWall[i].Update();
			}

			//クリア遷移

			player.ClearEffect(cam, player.SetGoalAndCheak(lowerLeft[stageNum], upperRight[stageNum]));

			if (!isClear && player.IsClear())
			{
				BGM.Stop();
				EnemyMgr::Instance()->StopSound();
				ClearSE.Play();
			}
			isClear = player.IsClear();

			if (!player.IsEffect() && isClear)
			{
				nowScene = CLEAR;
			}

			if (!isDead && player.IsDead())
			{
				BGM.Stop();
				EnemyMgr::Instance()->StopSound();
				GameoverSE.Play();
			}
			isDead = player.IsDead();
			if (!player.IsEffect() && isDead)
			{
				nowScene = GAMEOVER;
			}

			break;

#pragma endregion

#pragma region CLEAR_UPDATE

		case CLEAR:

			UpdateStart = false;

			if (resultFlag)
			{
				if (stickTrigger || (input->KeyTrigger(DIK_W) || input->KeyTrigger(DIK_S)))
				{
					if (input->LStick().y > 0.0f || input->KeyTrigger(DIK_W))
					{
						resultSelect--;
					}
					else if (input->LStick().y < 0.0f || input->KeyTrigger(DIK_S))
					{
						resultSelect++;
					}
					SelectSE.Play();
				}
				if (resultSelect >= 2)
				{
					resultSelect = 1;
				}
				if (resultSelect <= -1)
				{
					resultSelect = 0;
				}
				if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A) && !isTrigger)
				{
					sceneTransition.On();
					isTrigger = true;
					enterSE.Play();
				}

				//シーン遷移挟んだ移行処理
				if (sceneTransition.Change() && isTrigger)
				{
					easeTimer_START = 0.0f;
					mask.size = { 0,0 };
					easeTimer = 0.0f;
					isTrigger = false;
					stageNum += 1;
					if (resultSelect <= 0)
					{
						stageNum += 1;
						if (stageNum >= MAX_STAGE_NUM)
						{
							stageNum = 0;
						}
						player.Init(cam, StartPositions[stageNum]);
						cam.Init(XMFLOAT3(0, 250, 0), XMFLOAT3(0, 0, 0), StartPositions[stageNum], { 0,0,1 });

						if (stageNum == 0)
						{
							WallMgr::Instance()->Init(loomWalls);
							EnemyMgr::Instance()->Init(cam);
							EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, loomEnemyAngles, cam);
						}
						else if (stageNum == 1)
						{
							WallMgr::Instance()->Init(townWalls);
							EnemyMgr::Instance()->Init(cam);
							EnemyMgr::Instance()->Generate(townEnemyGeneratePos, townEnemyAngles, cam);
						}
						else if (stageNum == 2)
						{
							WallMgr::Instance()->Init(thirdStageWalls);
							EnemyMgr::Instance()->Init(cam);
							EnemyMgr::Instance()->Generate(thirdStageEnemyGeneratePos, thirdStageEnemyGenerateAngle, cam);
						}
						nowScene = GAME;
						enterSE.Play();
					}
					else
					{
						nowScene = STAGESELECT;
						//enterSE.Play();
					}
					resultFlag = false;
					resultSelect = 0;
				}
			}
			else
			{
				if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A))
				{
					resultFlag = true;
					ClearSE.Stop();
					OutBgm.Play();
				}
			}

			//HPのAlpha値を戻す
			if (hud_life_3.color.w != 1.0f)
			{
				hud_life_1.color.w = 1.0f;
				hud_life_2.color.w = 1.0f;
				hud_life_3.color.w = 1.0f;
			}

			break;

#pragma endregion

#pragma region GAMEOVER_UPDATE

		case GAMEOVER:

			UpdateStart = false;

			if (resultFlag)
			{
				if (stickTrigger || (input->KeyTrigger(DIK_W) || input->KeyTrigger(DIK_S)))
				{
					if (input->LStick().y > 0.0f || input->KeyTrigger(DIK_W))
					{
						resultSelect--;
					}
					else if (input->LStick().y < 0.0f || input->KeyTrigger(DIK_S))
					{
						resultSelect++;
					}
					SelectSE.Play();
				}
				if (resultSelect >= 2)
				{
					resultSelect = 1;
				}
				if (resultSelect <= -1)
				{
					resultSelect = 0;
				}
				if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A) && !isTrigger)
				{
					sceneTransition.On();
					isTrigger = true;
					enterSE.Play();
				}
				//シーン遷移挟んだ処理
				if (sceneTransition.Change() && isTrigger)
				{
					easeTimer_START = 0.0f;
					mask.size = { 0,0 };
					easeTimer = 0.0f;
					isTrigger = false;
					if (resultSelect <= 0)
					{
						player.Init(cam, StartPositions[stageNum]);
						cam.Init(XMFLOAT3(0, 250, 0), XMFLOAT3(0, 0, 0), StartPositions[stageNum], { 0,0,1 });

						if (stageNum == 0)
						{
							WallMgr::Instance()->Init(loomWalls);
							EnemyMgr::Instance()->Init(cam);
							EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, loomEnemyAngles, cam);
						}
						else if (stageNum == 1)
						{
							WallMgr::Instance()->Init(townWalls);
							EnemyMgr::Instance()->Init(cam);
							EnemyMgr::Instance()->Generate(townEnemyGeneratePos, townEnemyAngles, cam);
						}
						else if (stageNum == 2)
						{
							WallMgr::Instance()->Init(thirdStageWalls);
							EnemyMgr::Instance()->Init(cam);
							EnemyMgr::Instance()->Generate(thirdStageEnemyGeneratePos, thirdStageEnemyGenerateAngle, cam);
						}
						nowScene = GAME;
						enterSE.Play();
					}
					else
					{
						nowScene = STAGESELECT;
						//enterSE.Play();
					}
					resultFlag = false;
					resultSelect = 0;
				}
			}
			else
			{
				if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A))
				{
					resultFlag = true;
					OutBgm.Play();
				}
			}

			//HPのAlpha値を戻す
			if (hud_life_3.color.w != 1.0f)
			{
				hud_life_1.color.w = 1.0f;
				hud_life_2.color.w = 1.0f;
				hud_life_3.color.w = 1.0f;
			}

			break;


#pragma endregion

		default:
			break;
		}
		part.Update();

		//描画
		myDirectX->PreDraw();
		switch (oldScene)
		{

#pragma region TITLE_DRAW

		case TITLE:
			//debugText.Print("", window_width / 2 - 40, window_height / 2, 5);
			titleLogo.SpriteDraw();

			DepthReset();

			myObj->modelDraw(myModel->GetModel(), model3D->GetPipeLine());
			enemyObj->modelDraw(enemyModel->GetModel(), model3D->GetPipeLine());

			//文字
			if (input->isPadConnect()) { title_pad.SpriteDraw(); }
			else { title_keys.SpriteDraw(); }

			break;

#pragma endregion

#pragma region STAGESELECT_DRAW

		case STAGESELECT:

			if (input->isPadConnect())
			{
				stage_select_pad.SpriteDraw();
			}
			else
			{
				stage_select_keys.SpriteDraw();
			}

			debugText.Print("stageselect", 10, 10, 3);
			if (stageNum == 0)
			{
				debugText.Print("stage 1", 10, 100, 3);
			}
			if (stageNum == 1)
			{
				debugText.Print("stage 2", 10, 100, 3);
			}
			break;

#pragma endregion

#pragma region GAME_DRAW

		case GAME:

			//if (!player.IsHit()) box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			floor.modelDraw(boxModel.GetModel(), ModelPipeline::GetInstance()->GetPipeLine());

			player.Draw(model3D->GetPipeLine());
			for (int i = 0; i < outWall.size(); i++)
			{
				outWall[i].Draw();
			}
			//box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			//box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			//dome.modelDraw(domeModel.GetModel(), model3D->GetPipeLine());

			EnemyMgr::Instance()->Draw(model3D->GetPipeLine(), boxModel.GetModel());

			bomb.Draw();
			//for (int i = 0; i < loomWalls.size(); i++)
			//{
			//	//loomWalls[i].Draw();
			//}
			WallMgr::Instance()->Draw();

			part.Draw(particleGH);

			//攻撃範囲
			if (!player.IsDead() && !isClear) { hani.SpriteDraw(); }
			//再生
			if (!player.IsDead() && !isStop && !isClear) { spriteStart.SpriteDraw(); }
			//一時停止
			if (!player.IsDead() && isStop && !isClear) { spriteStop.SpriteDraw(); }

			//時間停止,解除時の演出
			if (!isClear && !isdead)mask.SpriteDraw();

			//開始時文字一定位置までは描画をしない
			if (UpdateStart)
			{
				if (!isClear && !player.IsDead())
				{
					//UI(簡易)
					if (input->isPadConnect())
					{
						hud_base_pad.SpriteDraw();
					}
					else
					{
						hud_base_keys.SpriteDraw();
					}

					hud_base_life.SpriteDraw();
					hud_timestate.SpriteDraw();


					//ミスったかも(下はみ出る)
					hud_stop.position.y = (32) * player.easeOutCubic(easeTimer) + 0;
					hud_play.position.y = (32) * player.easeOutCubic(easeTimer) + 32;

					if (easeTimer < 0.0f) { hud_stop.position.y = 0; hud_play.position.y = 32; }
					if (easeTimer > 1.0f) { hud_stop.position.y = 32; hud_play.position.y = 64; }

					if (isStop)
					{
						//hud_stop.position.y = 32;
						hud_stop.SpriteDraw();
						//hud_play_stop.position.y = (32 - 0) * player.easeOutCubic(easeTimer) + 32;
					}
					else
					{
						//hud_play.position.y = 32;
						//hud_play_stop.position.y = (64 - 96) * player.easeOutCubic(1 - easeTimer) + 64;
						hud_play.SpriteDraw();
					}

					/*if (player.GetHP() >= 1) { hud_life_1.SpriteDraw(); }
					if (player.GetHP() >= 2) { hud_life_2.SpriteDraw(); }
					if (player.GetHP() >= 3) { hud_life_3.SpriteDraw(); }*/

					//hud_stop.SpriteDraw();
					//hud_play.SpriteDraw();

					//hud_timestate.SpriteDraw();

					hud_life_1.SpriteDraw();
					hud_life_2.SpriteDraw();
					hud_life_3.SpriteDraw();

					//合体させたHUD
					//hud_play_stop.SpriteDraw();
				}
				//hud_play.SpriteDraw();
				//hud_stop.SpriteDraw();
			}

			go.SpriteDraw();

			break;

#pragma endregion

#pragma region CLEAR_DRAW

		case CLEAR:
			if (resultFlag)
			{
				clear_choise.SpriteDraw();
				if (resultSelect == 0)
				{
					clear_frame_up.SpriteDraw();
				}
				else
				{
					clear_frame_down.SpriteDraw();
				}
			}
			else
			{
				clearSprite.SpriteDraw();
			}
			break;

#pragma endregion

#pragma region GAMEOVER_DRAW

		case GAMEOVER:
			if (resultFlag)
			{
				dead_choise.SpriteDraw();
				if (resultSelect == 0)
				{
					dead_frame_up.SpriteDraw();
				}
				else
				{
					dead_frame_down.SpriteDraw();
				}
			}
			else
			{
				deadSprite.SpriteDraw();
			}
			break;

#pragma endregion

#pragma region CHOISE_DRAW

			//死亡後セレクト
		case CHOISE_DEAD:

			dead_choise.SpriteDraw();

			if (choiseNum)
			{
				dead_frame_up.SpriteDraw();
			}
			else
			{
				dead_frame_down.SpriteDraw();
			}

			//クリア後セレクト
		case CHOISE_CLEAR:

			clear_choise.SpriteDraw();

			if (choiseNum)
			{
				clear_frame_up.SpriteDraw();
			}
			else
			{
				clear_frame_down.SpriteDraw();
			}

#pragma endregion

		default:
			break;
		}

		//遷移Draw
		sceneTransition.Draw();

		//深度地リセット
		DepthReset();


		//.Print("Hello,DirectX!!", 200, 100);

		//debugText.Print("abcdefghijklmnopqrstuvwxyz", 200, 200, 2.0f);
		debugText.DrawAll();
		//④描画コマンドここまで

	//⑤リソースバリアを戻す
		myDirectX->PostDraw();
		// DirectX毎フレーム処理 ここまで
	}

	//xAudio2解放
	Sound::xAudioDelete();
	//ウィンドウクラスを登録解除
#pragma region WindowsAPI
	Win->end();
#pragma endregion
	return 0;
}
