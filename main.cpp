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
	Easing clearEase;
	clearEase.Init(60);
#pragma region particles
	ParticleManager::StaticInitialize(&cam);
#pragma endregion

#pragma region Init
	const float wallHeight = 1000;
	ParticleManager part;

	cam.Init(Vector3(0, 250, 0), Vector3(0, 0, 0), { 0,0,0 }, { 0,0,1 });
	float angle = 0.0f;	//カメラの回転角

	DebugText debugText;
	//モデル生成
	Model boxModel;
	Model domeModel;
	Model box2Model;
	//モデルのファイル読み込み
	boxModel.CreateModel("box");
	domeModel.CreateModel("skydome");
	box2Model.CreateModel("box2");

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
	float worldScale = 1.5f;

	std::vector<XMFLOAT3> StartPositions;
	const XMFLOAT3 STAGE_1 = Vector3(-450.1f, 0, 0) * worldScale;
	const XMFLOAT3 STAGE_2 = Vector3(-450.1f, 0, -125.1f) * worldScale;
	const XMFLOAT3 STAGE_3 = Vector3(-450.1f, 0, 0) * worldScale;
	const XMFLOAT3 STAGE_4 = Vector3(0, 0, 0) * worldScale;

	StartPositions.push_back(STAGE_1);
	StartPositions.push_back(STAGE_2);
	StartPositions.push_back(STAGE_3);
	StartPositions.push_back(STAGE_4);

#pragma endregion
#pragma region goal
	XMFLOAT3 lowerLeft[3];
	XMFLOAT3 upperRight[3];
	lowerLeft[0] = Vector3(400, 0, -50) * worldScale;
	upperRight[0] = Vector3(500, 0, 50) * worldScale;

	lowerLeft[1] = Vector3(400, 0, 100) * worldScale;
	upperRight[1] = Vector3(500, 0, 150) * worldScale;

	lowerLeft[2] = Vector3(400, 0, -150) * worldScale;
	upperRight[2] = Vector3(500, 0, -100) * worldScale;

#pragma endregion





	deadSprite.size = deadSprite.texSize;

	clearSprite.size = clearSprite.texSize;

	deadSprite.SpriteUpdate();
	clearSprite.SpriteUpdate();
	Player player(deadGraph, clearGraph, particleGH, DamageData);
	player.Init(cam, StartPositions[0]);
	bool isdead;
	int hp;
	Object3D goal;
	goal.Init(cam);
	goal.color = { 3, 1, 1,1 };


	Object3D floor;
	Vector3 floorBaseScale = { 1000.0f, 2.0f, 300.0f };
	floor.scale = { 1000.0f, 2.0f, 300.0f };
	floor.position = { 0.0f, -18.0f, 0.0f };
	floor.color = { 1.0f,0.6f ,0.5f ,1.0f };
	floor.Init(cam);


	Vector3 pillarScale = Vector3(30, wallHeight, 10) * worldScale;
	Vector3 LPillarScale = Vector3(50, wallHeight, 50) * worldScale;
	Vector3 wideWallScale = Vector3(200, wallHeight, 10) * worldScale;
	Vector3 heightWallScale = Vector3(10, wallHeight, 100) * worldScale;
	Vector3 LHeightWallScale = Vector3(80, wallHeight, 150) * worldScale;
	Vector3 LWidthWallScale = Vector3(150, wallHeight, 100) * worldScale;

#pragma endregion

	std::vector<Wall> outWall;
	outWall.resize(4);
	outWall[0].Init(cam, Vector3(-(floor.scale.x / 2), floor.position.y, 0.0f) * worldScale, Vector3(10, 1000, floor.scale.z) * worldScale);
	outWall[1].Init(cam, Vector3(0.0f, floor.position.y, -(floor.scale.z / 2)) * worldScale, Vector3(floor.scale.x, 1000, 10) * worldScale);
	outWall[2].Init(cam, Vector3(+(floor.scale.x / 2), floor.position.y, 0.0f) * worldScale, Vector3(10, 1000, floor.scale.z) * worldScale);
	outWall[3].Init(cam, Vector3(0.0f, floor.position.y, +(floor.scale.z / 2)) * worldScale, Vector3(floor.scale.x, 1000, 10) * worldScale);
#pragma region １面壁

	std::vector<Wall> loomWalls;
	const int stage_1_wallCount = 14;
	Wall wall[stage_1_wallCount];



	wall[0].Init(cam, Vector3(0.0f, floor.position.y, 0.0f), pillarScale);
	//入口奥
	wall[1].Init(cam, Vector3(-floor.scale.x / 2 + 100, floor.position.y, floor.scale.z / 2 - 100) * worldScale, wideWallScale);
	wall[2].Init(cam, Vector3(-floor.scale.x / 2 + 200, floor.position.y, floor.scale.z / 2 - 50) * worldScale, heightWallScale);
	//入口手前
	wall[3].Init(cam, Vector3(-floor.scale.x / 2 + 100, floor.position.y, -floor.scale.z / 2 + 100) * worldScale, wideWallScale);
	wall[4].Init(cam, Vector3(-floor.scale.x / 2 + 200, floor.position.y, -floor.scale.z / 2 + 50) * worldScale, heightWallScale);
	//入り口側中央柱
	wall[5].Init(cam, Vector3(-180, floor.position.y, 0) * worldScale, LHeightWallScale);
	//出口側二本柱
	wall[6].Init(cam, Vector3(+200, floor.position.y, -80) * worldScale, LPillarScale);
	wall[7].Init(cam, Vector3(+200, floor.position.y, +80) * worldScale, LPillarScale);
	//中間通路壁
	wall[8].Init(cam, Vector3(0, floor.position.y, floor.scale.z / 2 - 50) * worldScale, LWidthWallScale);
	wall[9].Init(cam, Vector3(0, floor.position.y, -floor.scale.z / 2 + 50) * worldScale, LWidthWallScale);
	//出口奥
	wall[10].Init(cam, Vector3(floor.scale.x / 2 - 100, floor.position.y, floor.scale.z / 2 - 100) * worldScale, wideWallScale);
	wall[11].Init(cam, Vector3(floor.scale.x / 2 - 200, floor.position.y, floor.scale.z / 2 - 50) * worldScale, heightWallScale);
	//出口手前
	wall[12].Init(cam, Vector3(+floor.scale.x / 2 - 100, floor.position.y, -floor.scale.z / 2 + 100) * worldScale, wideWallScale);
	wall[13].Init(cam, Vector3(+floor.scale.x / 2 - 200, floor.position.y, -floor.scale.z / 2 + 50) * worldScale, heightWallScale);

	for (int i = 0; i < stage_1_wallCount; i++)
	{
		loomWalls.push_back(wall[i]);
	}
	for (int i = 0; i < outWall.size(); i++)
	{
		loomWalls.push_back(outWall[i]);
	}
	WallMgr::Instance()->Init(loomWalls);
	std::vector<XMFLOAT3> loomEnemyGeneratePos;
	loomEnemyGeneratePos.push_back(Vector3(-255, 0, 105) * worldScale);
	loomEnemyGeneratePos.push_back(Vector3(-255, 0, -105) * worldScale);
	loomEnemyGeneratePos.push_back(Vector3(-85, 0, 0) * worldScale);
	loomEnemyGeneratePos.push_back(Vector3(130, 0, -50) * worldScale);
	loomEnemyGeneratePos.push_back(Vector3(130, 0, 50) * worldScale);
	loomEnemyGeneratePos.push_back(Vector3(260, 0, 0) * worldScale);
	std::vector<XMFLOAT3> enemyForwardVec;
	enemyForwardVec.push_back(XMFLOAT3(0, 0, -1));
	enemyForwardVec.push_back(XMFLOAT3(0, 0, 1));
	enemyForwardVec.push_back(XMFLOAT3(1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(-1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(-1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(1, 0, 0));
#pragma endregion

#pragma region 二面壁

	std::vector<Wall> townWalls;
	const int stage_2_wallCount = 12;

	Wall townWallData[stage_2_wallCount];

	//初期地点右上
	townWallData[0].Init(cam, Vector3(-floor.scale.x / 2 + 75, floor.position.y, -75.0f) * worldScale, Vector3(50, wallHeight, 50) * worldScale);

	//左壁沿い
	townWallData[1].Init(cam, Vector3(-floor.scale.x / 2 + 25, floor.position.y, 25.0f) * worldScale, Vector3(50, wallHeight, 50) * worldScale);
	//初期地点右右上
	townWallData[2].Init(cam, Vector3(-floor.scale.x / 2 + 250, floor.position.y, -75.0f) * worldScale, Vector3(50, wallHeight, 50) * worldScale);
	//初期地点側上部縦長壁
	townWallData[3].Init(cam, Vector3(-floor.scale.x / 2 + 180, floor.position.y, 40.0f) * worldScale, Vector3(50, wallHeight, 100) * worldScale);
	//中央上部左横長壁
	townWallData[4].Init(cam, Vector3(-140, floor.position.y, floor.scale.z / 2 - 80) * worldScale, Vector3(150, wallHeight, 75) * worldScale);
	//中央下部大壁
	townWallData[5].Init(cam, Vector3(-20, floor.position.y, -floor.scale.z / 2 + 50) * worldScale, Vector3(140, wallHeight, 100) * worldScale);
	//中央上部右大壁
	townWallData[6].Init(cam, Vector3(+100, floor.position.y, 60.0f) * worldScale, Vector3(150, wallHeight, 120) * worldScale);
	//右側下段壁
	townWallData[7].Init(cam, Vector3(+200, floor.position.y, -80) * worldScale, Vector3(150, wallHeight, 50) * worldScale);
	//右上隔離壁縦
	townWallData[8].Init(cam, Vector3(floor.scale.x / 2 - 230, floor.position.y, floor.scale.z / 2 - 50) * worldScale, Vector3(50, wallHeight, 100) * worldScale);
	//右下端壁
	townWallData[9].Init(cam, Vector3(floor.scale.x / 2 - 75, floor.position.y, -floor.scale.z / 2 + 100) * worldScale, Vector3(150, wallHeight, 200) * worldScale);
	//隔離部屋内壁
	townWallData[10].Init(cam, Vector3(floor.scale.x / 2 - 75, floor.position.y, floor.scale.z / 2 - 100) * worldScale, Vector3(150, wallHeight, 100) * worldScale);
	//壁抜け対策
	townWallData[11].Init(cam, Vector3(-floor.scale.x / 2, floor.position.y, -floor.scale.z / 2) * worldScale, Vector3(40, wallHeight, 40) * worldScale);

	townWalls.push_back(townWallData[0]);
	townWalls.push_back(townWallData[1]);
	townWalls.push_back(townWallData[2]);
	townWalls.push_back(townWallData[3]);
	townWalls.push_back(townWallData[4]);
	townWalls.push_back(townWallData[5]);
	townWalls.push_back(townWallData[6]);
	townWalls.push_back(townWallData[7]);
	townWalls.push_back(townWallData[8]);
	townWalls.push_back(townWallData[9]);
	townWalls.push_back(townWallData[10]);
	townWalls.push_back(townWallData[11]);

	for (int i = 0; i < stage_2_wallCount; i++)
	{
		townWalls.push_back(townWallData[i]);
	}
	for (int i = 0; i < outWall.size(); i++)
	{
		townWalls.push_back(outWall[i]);
	}

	WallMgr::Instance()->Init(townWalls);
	//wall


	EnemyMgr::Instance()->Init(cam);

	Scenes nowScene = TITLE;
	Scenes oldScene = nowScene;


	std::vector<XMFLOAT3> townEnemyGeneratePos;
	townEnemyGeneratePos.push_back(Vector3(-336, 0, -70) * worldScale);
	townEnemyGeneratePos.push_back(Vector3(-396, 0, 64) * worldScale);
	townEnemyGeneratePos.push_back(Vector3(-148, 0, -26) * worldScale);
	townEnemyGeneratePos.push_back(Vector3(-23, 0, 66) * worldScale);
	townEnemyGeneratePos.push_back(Vector3(88, 0, -80) * worldScale);
	townEnemyGeneratePos.push_back(Vector3(304, 0, 0) * worldScale);




#pragma endregion

#pragma region 3面壁
	std::vector<Wall> thirdStageWalls;
	const int stage_3_wallCount = 9;

	Wall thirdStageWallData[stage_3_wallCount];

	//入口壁
	thirdStageWallData[0].Init(cam, Vector3(-floor.scale.x / 2 + 50, -20, floor.scale.z / 2 - 50) * worldScale, Vector3(100, wallHeight, 100) * worldScale);
	thirdStageWallData[1].Init(cam, Vector3(-floor.scale.x / 2 + 50, -20, -floor.scale.z / 2 + 50) * worldScale, Vector3(100, wallHeight, 100) * worldScale);

	//鍵壁
	thirdStageWallData[2].Init(cam, Vector3(-floor.scale.x / 2 + 150, -20, -60) * worldScale, Vector3(150, wallHeight, 20) * worldScale);
	thirdStageWallData[3].Init(cam, Vector3(-floor.scale.x / 2 + 215, -20, 0) * worldScale, Vector3(20, wallHeight, 100) * worldScale);

	//中央縦壁
	thirdStageWallData[4].Init(cam, Vector3(-floor.scale.x / 2 + 350, -20, 40) * worldScale, Vector3(40, wallHeight, 220) * worldScale);

	//中央でっぱり
	thirdStageWallData[5].Init(cam, Vector3(-floor.scale.x / 2 + 410, -20, +60) * worldScale, Vector3(80, wallHeight, 20) * worldScale);

	//ゴール側竪壁
	thirdStageWallData[6].Init(cam, Vector3(-floor.scale.x / 2 + 700, -20, +60) * worldScale, Vector3(300, wallHeight, 20) * worldScale);


	//右上横壁
	thirdStageWallData[7].Init(cam, Vector3(-floor.scale.x / 2 + 650, -20, -55) * worldScale, Vector3(40, wallHeight, 220) * worldScale);

	//ゴール壁
	thirdStageWallData[8].Init(cam, Vector3(floor.scale.x / 2 - 80, -20, -100) * worldScale, Vector3(200, wallHeight, 20) * worldScale);
	for (int i = 0; i < stage_3_wallCount; i++)
	{
		thirdStageWalls.push_back(thirdStageWallData[i]);
	}


	for (int i = 0; i < outWall.size(); i++)
	{
		thirdStageWalls.push_back(outWall[i]);
	}

	std::vector<XMFLOAT3> thirdStageEnemyGeneratePos;
	thirdStageEnemyGeneratePos.push_back(Vector3(-346, 0, 97) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(-222, 0, 97) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(-222, 0, -7) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(-365, 0, -106) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(2, 0, -100) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(-89, 0, 10) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(-89, 0, -55) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(87, 0, 10) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(87, 0, -55) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(-89, 0, 108) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(87, 0, 108) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(228, 0, 108) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(430, 0, 107) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(430, 0, -47) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(342, 0, -47) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(211, 0, 16) * worldScale);
	thirdStageEnemyGeneratePos.push_back(Vector3(221, 0, -102) * worldScale);
	std::vector<XMFLOAT3> thirdStageEnemyGenerateAngle;
	thirdStageEnemyGenerateAngle.resize(thirdStageEnemyGeneratePos.size());
	for (int i = 0; i < thirdStageEnemyGenerateAngle.size(); i++)
	{
		thirdStageEnemyGenerateAngle[i] = up;
	}

	floor.scale = { floorBaseScale.x * worldScale, 2.0f, floorBaseScale.z * worldScale };
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

	//範囲改
	int ATTACK_ANIM1 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/ATTACK/ATTACK_ANIM1.png");
	int ATTACK_ANIM2 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/ATTACK/ATTACK_ANIM2.png");
	int ATTACK_ANIM3 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/ATTACK/ATTACK_ANIM3.png");
	int ATTACK_ANIM4 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/ATTACK/ATTACK_ANIM4.png");
	int ATTACK_AREA = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/ATTACK/ATTACK_AREA.png");

	Sprite attack_anim[4];
	Sprite attack_area;

	attack_anim[0].Init(ATTACK_ANIM1, XMFLOAT2(0.5f, 1.0f));
	attack_anim[1].Init(ATTACK_ANIM2, XMFLOAT2(0.5f, 1.0f));
	attack_anim[2].Init(ATTACK_ANIM3, XMFLOAT2(0.5f, 1.0f));
	attack_anim[3].Init(ATTACK_ANIM4, XMFLOAT2(0.5f, 1.0f));
	attack_area.Init(ATTACK_AREA, XMFLOAT2(0.5f, 1.0f));

	attack_anim[0].position = { window_width / 2,window_height / 2,0 };
	attack_anim[1].position = { window_width / 2,window_height / 2,0 };
	attack_anim[2].position = { window_width / 2,window_height / 2,0 };
	attack_anim[3].position = { window_width / 2,window_height / 2,0 };
	attack_area.position = { window_width / 2,window_height / 2,0 };

	attack_anim[0].color.w = 0.7f;
	attack_anim[1].color.w = 0.7f;
	attack_anim[2].color.w = 0.7f;
	attack_anim[3].color.w = 0.7f;
	attack_area.color.w = 0.7f;

	//アニメーション用タイマー
	int animationTimer = 0;
	int ANIMATION_TIMER = 2;
	int number = 0;
	bool isAnimation = false;


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
	int HUD_CONTROLL_PAD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/HUD_BASE_PAD_2.png");
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
	int BACK = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/back_2.png");
	int STAGE_SELECT = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_SELECT.png");
	int STAGE_SELECT_GRAPH_PAD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/SELECT_ENTER_PAD.png");
	int STAGE_SELECT_GRAPH_KEYBOARD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/SELECT_ENTER_KEY.png");

	Sprite back;
	Sprite stage_select;
	Sprite stage_select_pad;
	Sprite stage_select_keys;

	back.Init(BACK);
	stage_select.Init(STAGE_SELECT);
	stage_select_pad.Init(STAGE_SELECT_GRAPH_PAD);
	stage_select_keys.Init(STAGE_SELECT_GRAPH_KEYBOARD);

	back.size = { window_width,window_height };
	stage_select.size = { 800,160 };
	stage_select_pad.size = { 640, 320 };
	stage_select_keys.size = { 640, 320 };

	back.position = { window_width / 2,window_height / 2,0 };
	stage_select.position = { window_width / 2,80,0 };
	stage_select_pad.position = { window_width / 2,620 ,0 };
	stage_select_keys.position = { window_width / 2,620,0 };

	back.SpriteUpdate();
	stage_select.SpriteUpdate();

	int MASK = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/mask_circle.png");
	Sprite mask;
	mask.Init(MASK);
	mask.size = { 0,0 };
	mask.position = { window_width / 2,window_height / 2,0 };
	mask.color.w = 0.1f;

	float easeTimer = 0.0f;

	//番号
	int STAGE1 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_1.png");
	int STAGE2 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_2.png");
	int STAGE3 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_3.png");
	int STAGE4 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_4.png");
	int STAGE5 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_5.png");
	int STAGE_FRAME = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/STAGE_FRAME.png");

	Sprite stage_1;
	Sprite stage_2;
	Sprite stage_3;
	Sprite stage_4;
	Sprite stage_5;
	Sprite stage_frame;

	stage_1.Init(STAGE1);
	stage_2.Init(STAGE2);
	stage_3.Init(STAGE3);
	stage_4.Init(STAGE4);
	stage_5.Init(STAGE5);
	stage_frame.Init(STAGE_FRAME);

	stage_1.size = { 192,192 };
	stage_2.size = { 192,192 };
	stage_3.size = { 192,192 };
	stage_4.size = { 192,192 };
	stage_5.size = { 192,192 };
	stage_frame.size = { 192,192 };

	float half_Width = window_width / 2;
	float half_height = window_height / 2;
	float mini_Width = window_width / 2.5f;
	float mini_height = window_height / 4;

	float stageNumF = 0.0f;
	float stageEase = 0.0f;
	bool isMove = false;
	bool direction = false;
	bool trigger = false;

	stage_1.position = { half_Width + mini_Width * 0.0f,half_height,0 };
	stage_2.position = { half_Width + mini_Width * 1.0f,half_height,0 };
	stage_3.position = { half_Width + mini_Width * 2.0f,half_height,0 };
	stage_4.position = { half_Width + mini_Width * 3.0f,half_height,0 };
	stage_5.position = { half_Width + mini_Width * 4.0f,half_height,0 };
	stage_frame.position = { half_Width,half_height,0 };

	//ステージレイアウト
	int LAYOUT_1 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/Layout_1.png");
	int LAYOUT_2 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/Layout_2.png");
	int LAYOUT_3 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/STAGE_SELECT/Layout_3.png");

	Sprite layout_1;
	Sprite layout_2;
	Sprite layout_3;

	layout_1.Init(LAYOUT_1);
	layout_2.Init(LAYOUT_2);
	layout_3.Init(LAYOUT_3);

	float late = 1.6f;

	layout_1.size = { 600 * late,300 * late };
	layout_2.size = { 600 * late,300 * late };
	layout_3.size = { 600 * late,300 * late };

	/*layout_1.size = { 600.0f,300.0f };
	layout_2.size = { 600.0f,300.0f };
	layout_3.size = { 600.0f,300.0f };*/

	layout_1.position = { half_Width,half_height,0 };
	layout_2.position = { half_Width,half_height,0 };
	layout_3.position = { half_Width,half_height,0 };

	//ギア
	int GEAR_1 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_1.png");
	int GEAR_2 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_2.png");
	int GEAR_3 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_3.png");
	int GEAR_4 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_4.png");
	int GEAR_5 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_5.png");
	int GEAR_6 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_6.png");
	int GEAR_7 = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/GEARS/GEAR_7.png");

	Sprite gears[7];

	gears[0].Init(GEAR_1);
	gears[1].Init(GEAR_2);
	gears[2].Init(GEAR_3);
	gears[3].Init(GEAR_4);
	gears[4].Init(GEAR_5);
	gears[5].Init(GEAR_6);
	gears[6].Init(GEAR_7);

	float gear_late = 5.0f;

	gears[0].size = { 96 * gear_late,96 * gear_late };
	gears[1].size = { 96 * gear_late,96 * gear_late };
	gears[2].size = { 96 * gear_late,96 * gear_late };
	gears[3].size = { 96 * gear_late,96 * gear_late };
	gears[4].size = { 96 * gear_late,96 * gear_late };
	gears[5].size = { 96 * gear_late,96 * gear_late };
	gears[6].size = { 96 * gear_late,96 * gear_late };

	gears[0].position = { 0,0,0 };
	gears[1].position = { 1000,720,0 };
	gears[2].position = { 400,0,0 };
	gears[3].position = { 1280,100,0 };
	gears[4].position = { 0,600,0 };
	gears[5].position = { 300,720,0 };
	gears[6].position = { 800,0,0 };

	/*----------その他----------*/
	//開始時の文字
	int GO = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/go.png");
	Sprite go;
	go.Init(GO, XMFLOAT2(0.0f, 0.0f));
	go.size = { 640,180 };
	go.position = { 320.0f,-180.0f,0 };

	float easeTimer_START = 1.0f;
	bool UpdateStart = false;

	//死亡後、クリア後の文字
	int NEXT_PAD = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/NEXT_CONTROLLER.png");
	int NEXT_KEYS = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/HUD/NEXT_KEYBOARD.png");

	Sprite next_pad;
	Sprite next_keys;

	next_pad.Init(NEXT_PAD);
	next_keys.Init(NEXT_KEYS);

	next_pad.size = { 240,64 };
	next_keys.size = { 240,64 };

	next_pad.position = { window_width - 200,window_height - 64,0 };
	next_keys.position = { window_width - 200,window_height - 64,0 };

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

		//範囲改
		attack_anim[0].rotation = player.GetAngle() + 90.0f;
		attack_anim[1].rotation = player.GetAngle() + 90.0f;
		attack_anim[2].rotation = player.GetAngle() + 90.0f;
		attack_anim[3].rotation = player.GetAngle() + 90.0f;
		attack_area.rotation = player.GetAngle() + 90.0f;

		attack_anim[0].position = { width / 2 + (CtoP.x * 2) ,height / 2 - (CtoP.z * 2),0 };
		attack_anim[1].position = { width / 2 + (CtoP.x * 2) ,height / 2 - (CtoP.z * 2),0 };
		attack_anim[2].position = { width / 2 + (CtoP.x * 2) ,height / 2 - (CtoP.z * 2),0 };
		attack_anim[3].position = { width / 2 + (CtoP.x * 2) ,height / 2 - (CtoP.z * 2),0 };
		attack_area.position = { width / 2 + (CtoP.x * 2) ,height / 2 - (CtoP.z * 2),0 };

		if (!isAnimation)
		{
			if (number != 0) { number = 0; }
			isAnimation = player.GetAttackFlag();
		}

		if (isAnimation)
		{
			if (animationTimer < ANIMATION_TIMER) { animationTimer++; }
			else { number++; animationTimer = 0; }

			if (number >= 3 && animationTimer >= ANIMATION_TIMER) { isAnimation = false; number = 0; }
		}

		if (!player.IsDead())
		{
			if (isAnimation)
			{
				attack_anim[0].SpriteUpdate();
				attack_anim[1].SpriteUpdate();
				attack_anim[2].SpriteUpdate();
				attack_anim[3].SpriteUpdate();
			}
			else { attack_area.SpriteUpdate(); }

		}

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

		//ギア処理
		if (isMove)
		{
			if (!direction)
			{
				for (int i = 0; i < 7; i++)
				{
					if (i % 2 == 0) { gears[i].rotation += 5.0f; }
					else { gears[i].rotation -= 5.0f; }
					//gears[i].SpriteUpdate();
				}
			}

			else
			{
				for (int i = 0; i < 7; i++)
				{
					if (i % 2 == 0) { gears[i].rotation -= 5.0f; }
					else { gears[i].rotation += 5.0f; }
					//gears[i].SpriteUpdate();
				}
			}
		}

		for (int i = 0; i < 7; i++) { gears[i].SpriteUpdate(); }

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

			layout_1.color.w = 0;
			layout_2.color.w = 0;
			layout_3.color.w = 0;

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
				trigger = true;

				if (input->KeyTrigger(DIK_A) || input->LStick().x < 0.0f)
				{
					stageNum--;
					if (!isMove && stageNum >= 0) { direction = false; isMove = true; stageEase = 0.0f; }
				}
				if (input->KeyTrigger(DIK_D) || input->LStick().x > 0.0f)
				{
					stageNum++;
					if (!isMove && stageNum <= MAX_STAGE_NUM - 1) { direction = true; isMove = true; stageEase = 0.0f; }
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

			//ステージ選択画面処理
			//左(1->0)
			if (!direction && stageNum >= 0 && isMove)
			{
				stageNumF = (stageNum - (stageNum + 1)) * player.easeOutCubic(stageEase) + (stageNum + 1);
				if (stageEase < 1.0f) { stageEase += 0.1f; }
				else { isMove = false; stageEase = 0.0f; stageNumF = stageNum; }
			}

			//右(0->1)
			else if (direction && stageNum < MAX_STAGE_NUM && isMove)
			{
				stageNumF = (stageNum - (stageNum - 1)) * player.easeOutCubic(stageEase) + (stageNum - 1);
				if (stageEase < 1.0f) { stageEase += 0.1f; }
				else { isMove = false; stageEase = 0.0f; stageNumF = stageNum; }
			}

			stage_1.position = { half_Width + mini_Width * (-stageNumF + 0),half_height,0 };
			stage_2.position = { half_Width + mini_Width * (-stageNumF + 1),half_height,0 };
			stage_3.position = { half_Width + mini_Width * (-stageNumF + 2),half_height,0 };
			stage_4.position = { half_Width + mini_Width * (-stageNumF + 3),half_height,0 };
			stage_5.position = { half_Width + mini_Width * (-stageNumF + 4),half_height,0 };

			stage_1.SpriteUpdate();
			stage_2.SpriteUpdate();
			stage_3.SpriteUpdate();
			stage_4.SpriteUpdate();
			stage_5.SpriteUpdate();
			stage_frame.SpriteUpdate();

			//選択画面,文字
			back.SpriteUpdate();
			stage_select.SpriteUpdate();

			if (input->isPadConnect())
			{
				if (stage_select_pad.color.w >= 1.5f) { isAdd = false; }
				else if (stage_select_pad.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { stage_select_pad.color.w -= 0.02f; }
				else { stage_select_pad.color.w += 0.02f; }

				stage_select_pad.SpriteUpdate();
			}
			else
			{
				if (stage_select_keys.color.w >= 1.5f) { isAdd = false; }
				else if (stage_select_keys.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { stage_select_keys.color.w -= 0.02f; }
				else { stage_select_keys.color.w += 0.02f; }

				stage_select_keys.SpriteUpdate();
			}

			layout_1.SpriteUpdate();
			layout_2.SpriteUpdate();
			layout_3.SpriteUpdate();

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
				Vector3 goalScale(upperRight[stageNum]);
				goalScale -= Vector3(lowerLeft[stageNum]);
				goal.position = Vector3(lowerLeft[stageNum]) + (goalScale / 2);
				goal.position.y = floor.position.y + floor.scale.y + 1.0f;
				goal.scale = goalScale;
				goalScale.y = 0.1;

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
			goal.Update(cam);
			box.position = { box.position.x + moveSpeed.x,box.position.y + moveSpeed.y ,box.position.z + moveSpeed.z };

			dome.Update(cam);
			floor.Update(cam);
			{
				Sphere pSphere;
				pSphere.center = XMLoadFloat3(&player.GetPos());
				pSphere.radius = 16;
				if (!player.IsClear() && !player.IsDead())
				{
					EnemyMgr::Instance()->Update(cam, player.GetPos(), player.GetAngle(), player.GetStopTimeFlag(), player.GetAttackFlag());
				}
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

			if (isClear)
			{
			float rate = clearEase.Do(Easing::Out, Easing::Cubic);
				Vector3 tmp = cam.position;
				Vector3 endPos = player.GetPos();
				endPos.x = upperRight[stageNum].x - player.GetPos().x;
				endPos.z = 0;
				endPos.y = 250- cam.position.y;
				cam.eye = (endPos * rate) + (cam.eye * (1- rate));
				cam.up = (Vector3(0, 1, 0) * rate) + (Vector3(cam.up) * (1 - rate));
			}
			if (!player.IsEffect() && isClear)
			{
				nowScene = CLEAR;
				clearEase.Reset();
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

			//死亡後、クリア後の文字
			if (input->isPadConnect())
			{
				if (next_pad.color.w >= 1.5f) { isAdd = false; }
				else if (next_pad.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { next_pad.color.w -= 0.02f; }
				else { next_pad.color.w += 0.02f; }

				next_pad.SpriteUpdate();
			}
			else
			{
				if (next_keys.color.w >= 1.5f) { isAdd = false; }
				else if (next_keys.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { next_keys.color.w -= 0.02f; }
				else { next_keys.color.w += 0.02f; }

				next_keys.SpriteUpdate();
			}

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
						Vector3 goalScale(upperRight[stageNum]);
						goalScale -= Vector3(lowerLeft[stageNum]);
						goal.position = Vector3(lowerLeft[stageNum]) + (goalScale / 2);
						goal.position.y = floor.position.y + floor.scale.y + 1.0f;
						goal.scale = goalScale;
						goalScale.y = 0.1;
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

			//死亡後、クリア後の文字
			if (input->isPadConnect())
			{
				if (next_pad.color.w >= 1.5f) { isAdd = false; }
				else if (next_pad.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { next_pad.color.w -= 0.02f; }
				else { next_pad.color.w += 0.02f; }

				next_pad.SpriteUpdate();
			}
			else
			{
				if (next_keys.color.w >= 1.5f) { isAdd = false; }
				else if (next_keys.color.w <= -0.5f) { isAdd = true; }

				if (!isAdd) { next_keys.color.w -= 0.02f; }
				else { next_keys.color.w += 0.02f; }

				next_keys.SpriteUpdate();
			}

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
						Vector3 goalScale(upperRight[stageNum]);
						goalScale -= Vector3(lowerLeft[stageNum]);
						goal.position = Vector3(lowerLeft[stageNum]) + (goalScale / 2);
						goal.position.y = floor.position.y + floor.scale.y + 1.0f;
						goal.scale = goalScale;
						goalScale.y = 0.1;
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

			back.SpriteDraw();

			for (int i = 0; i < 7; i++)
			{
				gears[i].SpriteDraw();
			}

			debugText.Print("stageselect", 10, 10, 3);
			if (stageNum == 0 && !isMove)
			{
				debugText.Print("stage 1", 10, 100, 3);
				layout_2.color.w = 0;
				layout_3.color.w = 0;
				if (layout_1.color.w < 1.0f) { layout_1.color.w += 0.1f; }
				layout_1.SpriteDraw();
			}
			if (stageNum == 1 && !isMove)
			{
				debugText.Print("stage 2", 10, 100, 3);
				layout_1.color.w = 0;
				layout_3.color.w = 0;
				if (layout_2.color.w < 1.0f) { layout_2.color.w += 0.1f; }
				layout_2.SpriteDraw();
			}
			if (stageNum == 2 && !isMove)
			{
				debugText.Print("stage 3", 10, 100, 3);
				layout_1.color.w = 0;
				layout_2.color.w = 0;
				if (layout_3.color.w < 1.0f) { layout_3.color.w += 0.1f; }
				layout_3.SpriteDraw();
			}

			//ステージ選択番号
			stage_1.SpriteDraw();
			stage_2.SpriteDraw();
			stage_3.SpriteDraw();
			stage_4.SpriteDraw();
			stage_5.SpriteDraw();
			stage_frame.SpriteDraw();

			if (input->isPadConnect())
			{
				stage_select_pad.SpriteDraw();
			}
			else
			{
				stage_select_keys.SpriteDraw();
			}

			stage_select.SpriteDraw();

			break;

#pragma endregion

#pragma region GAME_DRAW

		case GAME:

			//if (!player.IsHit()) box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			floor.modelDraw(boxModel.GetModel(), ModelPipeline::GetInstance()->GetPipeLine());
			goal.modelDraw(boxModel.GetModel(), ModelPipeline::GetInstance()->GetPipeLine());
			for (int i = 0; i < outWall.size(); i++)
			{
				outWall[i].Draw();
			}
			//box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			//box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			//dome.modelDraw(domeModel.GetModel(), model3D->GetPipeLine());

			EnemyMgr::Instance()->Draw(model3D->GetPipeLine(), box2Model.GetModel(), particleGH);

			bomb.Draw();
			//for (int i = 0; i < loomWalls.size(); i++)
			//{
			//	//loomWalls[i].Draw();
			//}
			WallMgr::Instance()->Draw();
			player.Draw(model3D->GetPipeLine());

			part.Draw(particleGH);

			//攻撃範囲
			//if (!player.IsDead() && !isClear) { hani.SpriteDraw(); }

			//範囲改
			if (isAnimation)
			{
				if (!player.IsDead() && !isClear)
				{
					attack_anim[number].SpriteDraw();
				}
			}

			else { if (!player.IsDead() && !isClear) { attack_area.SpriteDraw(); } }

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

			//文字
			if (input->isPadConnect()) { next_pad.SpriteDraw(); }
			else { next_keys.SpriteDraw(); }

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

			//文字
			if (input->isPadConnect()) { next_pad.SpriteDraw(); }
			else { next_keys.SpriteDraw(); }

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
