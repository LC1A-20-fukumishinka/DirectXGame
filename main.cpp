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
const XMFLOAT3 STAGE_1 = { -450,0,0 };
const XMFLOAT3 STAGE_2 = { 0,0,0 };
const XMFLOAT3 STAGE_3 = { 0,0,0 };
const XMFLOAT3 STAGE_4 = { 0,0,0 };

enum Scenes
{
	TITLE,
	STAGESELECT,
	GAME,
	CLEAR,
	GAMEOVER
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//WindowsAPI初期化処理
#pragma region WindowsAPI

	WinAPI *Win = WinAPI::GetInstance();

	Win->Init(window_width, window_height);
#pragma endregion
#pragma region sound(xAudioInstance)

	Sound::StaticInitialize();
	int alarm = Sound::SoundLoadWave("Resources/Alarm01.wav");

	Sound voice(alarm);

#pragma endregion

	//DirectX初期化処理 ここまで
	MyDirectX *myDirectX = MyDirectX::GetInstance();

	IGraphicsPipeline *Pipe3D = GraphicsPipeline3D::GetInstance();
	IGraphicsPipeline *model3D = ModelPipeline::GetInstance();

#pragma region DirectInput
	Input *input = Input::GetInstance();
	input->Init(Win->w, Win->hwnd);
#pragma endregion



	Camera cam;

#pragma region particles
	ParticleManager::StaticInitialize(&cam);
#pragma endregion

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


	Player player;
	player.Init(cam, STAGE_1);



	Object3D floor;
	floor.scale = { 1000.0f, 2.0f, 300.0f };
	floor.position = { 0.0f, -20.0f, 0.0f };
	floor.color = { 3.0f,2.0f ,2.0f ,1.0f };
	floor.Init(cam);


	DirectX::XMFLOAT3 pillarScale = { 10, 500, 10 };
	DirectX::XMFLOAT3 LPillarScale = { 50, 500, 50 };
	DirectX::XMFLOAT3 wideWallScale = { 200, 500, 10 };
	DirectX::XMFLOAT3 heightWallScale = { 10, 500, 100 };
	DirectX::XMFLOAT3 LHeightWallScale = { 80, 500, 150 };
	DirectX::XMFLOAT3 LWidthWallScale = { 150, 500, 100 };
#pragma region loom

	std::vector<Wall> loomWalls;
	Wall wall[14];



	wall[0].Init(cam, { 0.0f,floor.position.y ,0.0f }, pillarScale, { pillarScale.x / 4, 10, pillarScale.z / 4 });
	loomWalls.push_back(wall[0]);

	//入口奥
	wall[1].Init(cam, { -floor.scale.x / 2 + 100,floor.position.y ,floor.scale.z / 2 - 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	loomWalls.push_back(wall[1]);
	wall[2].Init(cam, { -floor.scale.x / 2 + 200,floor.position.y ,floor.scale.z / 2 - 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	loomWalls.push_back(wall[2]);

	//入口手前
	wall[3].Init(cam, { -floor.scale.x / 2 + 100,floor.position.y ,-floor.scale.z / 2 + 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	loomWalls.push_back(wall[3]);
	wall[4].Init(cam, { -floor.scale.x / 2 + 200,floor.position.y ,-floor.scale.z / 2 + 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	loomWalls.push_back(wall[4]);

	//入り口側中央柱
	wall[5].Init(cam, { -180,floor.position.y ,0 }, LHeightWallScale, { LHeightWallScale.x / 2, 10, LHeightWallScale.z / 2 });
	loomWalls.push_back(wall[5]);

	//出口側二本柱
	wall[6].Init(cam, { +200,floor.position.y ,-80 }, LPillarScale, { LPillarScale.x / 2, 10, LPillarScale.z / 2 });
	loomWalls.push_back(wall[6]);
	wall[7].Init(cam, { +200,floor.position.y ,+80 }, LPillarScale, { LPillarScale.x / 2, 10, LPillarScale.z / 2 });
	loomWalls.push_back(wall[7]);

	//中間通路壁
	wall[8].Init(cam, { 0,floor.position.y ,floor.scale.z / 2 - 50 }, LWidthWallScale, { LWidthWallScale.x / 2, 10, LWidthWallScale.z / 2 });
	loomWalls.push_back(wall[8]);
	wall[9].Init(cam, { 0,floor.position.y ,-floor.scale.z / 2 + 50 }, LWidthWallScale, { LWidthWallScale.x / 2, 10, LWidthWallScale.z / 2 });
	loomWalls.push_back(wall[9]);

	//出口奥
	wall[10].Init(cam, { floor.scale.x / 2 - 100,floor.position.y ,floor.scale.z / 2 - 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	loomWalls.push_back(wall[10]);
	wall[11].Init(cam, { floor.scale.x / 2 - 200,floor.position.y ,floor.scale.z / 2 - 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	loomWalls.push_back(wall[11]);

	//出口手前
	wall[12].Init(cam, { +floor.scale.x / 2 - 100,floor.position.y ,-floor.scale.z / 2 + 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	loomWalls.push_back(wall[12]);
	wall[13].Init(cam, { +floor.scale.x / 2 - 200,floor.position.y ,-floor.scale.z / 2 + 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	loomWalls.push_back(wall[13]);

	WallMgr::Instance()->Init(loomWalls);

#pragma endregion
	std::vector<Wall> townWalls;
	Wall townWallData[11];

	//初期地点右上
	townWallData[0].Init(cam, { -floor.scale.x / 2 + 75,floor.position.y, -75.0f }, { 50, 500, 50 }, { 25, 10, 25 });
	townWalls.push_back(townWallData[0]);

	//左壁沿い
	townWallData[1].Init(cam, { -floor.scale.x / 2 + 25,floor.position.y ,25.0f }, { 50, 500, 50 }, { 25, 10, 25 });
	townWalls.push_back(townWallData[1]);

	//初期地点右右上
	townWallData[2].Init(cam, { -floor.scale.x / 2 + 250,floor.position.y, -75.0f }, { 50, 500, 50 }, { 25, 10, 25 });
	townWalls.push_back(townWallData[2]);

	//初期地点側上部縦長壁
	townWallData[3].Init(cam, { -floor.scale.x / 2 + 180,floor.position.y, 40.0f }, { 50, 500, 100 }, { 25, 10, 50 });
	townWalls.push_back(townWallData[3]);

	//中央上部左横長壁
	townWallData[4].Init(cam, { -140,floor.position.y ,floor.scale.z / 2 - 80 }, { 150, 500, 75 }, { 75, 10, 37.5f });
	townWalls.push_back(townWallData[4]);

	//中央下部大壁
	townWallData[5].Init(cam, { -20,floor.position.y ,-floor.scale.z / 2 + 50 }, { 140, 500, 100 }, { 70, 10, 50 });
	townWalls.push_back(townWallData[5]);

	//中央上部右大壁
	townWallData[6].Init(cam, { +100,floor.position.y ,60.0f }, { 150, 500, 120 }, { 75, 10, 60 });
	townWalls.push_back(townWallData[6]);

	//右側下段壁
	townWallData[7].Init(cam, { +200,floor.position.y ,-80 }, { 150, 500, 50 }, { 75.0f, 10, 25 });
	townWalls.push_back(townWallData[7]);

	//右上隔離壁縦
	townWallData[8].Init(cam, { floor.scale.x / 2 - 230,floor.position.y ,floor.scale.z / 2 - 50 }, { 50, 500, 100 }, { 25, 10, 50 });
	townWalls.push_back(townWallData[8]);
	//右下端壁
	townWallData[9].Init(cam, { floor.scale.x / 2 - 75,floor.position.y ,-floor.scale.z / 2 + 100 }, { 150, 500, 200 }, { 75, 10, 100 });
	townWalls.push_back(townWallData[9]);

	//隔離部屋内壁
	townWallData[10].Init(cam, { floor.scale.x / 2 - 75,floor.position.y ,floor.scale.z / 2 - 100 }, { 150, 500, 100 }, { 75, 10, 50 });
	townWalls.push_back(townWallData[10]);

	WallMgr::Instance()->Init(townWalls);
	//wall
	std::vector<Wall> outWall;
	outWall.resize(4);
	outWall[0].Init(cam, { -(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 500, floor.scale.z }, { 10.0f / 2, 10, floor.scale.z / 2 });
	outWall[1].Init(cam, { 0.0f,floor.position.y ,-(floor.scale.z / 2) }, { floor.scale.x, 500, 10 }, { floor.scale.x / 2 , 10, 10.0f / 2 });
	outWall[2].Init(cam, { +(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 500, floor.scale.z }, { 10.0f / 2, 10, floor.scale.z / 2 });
	outWall[3].Init(cam, { 0.0f,floor.position.y ,+(floor.scale.z / 2) }, { floor.scale.x, 500, 10 }, { floor.scale.x / 2, 10, 10.0f / 2 });

	EnemyMgr::Instance()->Init(cam);

	Scenes nowScene = TITLE;
	std::vector<XMFLOAT3> loomEnemyGeneratePos;
	loomEnemyGeneratePos.push_back(XMFLOAT3{ -255, 0, 105 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ -255, 0, -105 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ -85, 0, 0 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ 130, 0, -50 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ 130, 0, 50 });
	loomEnemyGeneratePos.push_back(XMFLOAT3{ 260, 0, 0 });
	std::vector<XMFLOAT3> townEnemyGeneratePos;
	townEnemyGeneratePos.push_back(XMFLOAT3{ -336, 0, -70 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ -396, 0, 64 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ -148, 0, -26 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ -23, 0, 66 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ 88, 0, -80 });
	townEnemyGeneratePos.push_back(XMFLOAT3{ 304, 0, 0 });
	//std::vector<XMFLOAT3> enemyGeneratePos;
	//enemyGeneratePos.push_back(XMFLOAT3{ -255, 0, 105 });
	//enemyGeneratePos.push_back(XMFLOAT3{ -255, 0, -105 });
	//enemyGeneratePos.push_back(XMFLOAT3{ -85, 0, 0 });
	//enemyGeneratePos.push_back(XMFLOAT3{ 130, 0, -50 });
	////enemyGeneratePos.push_back(XMFLOAT3{ , 0, -30 });
	//enemyGeneratePos.push_back(XMFLOAT3{ 130, 0, 50 });
	////enemyGeneratePos.push_back(XMFLOAT3{ 260, 0, -120 });
	//enemyGeneratePos.push_back(XMFLOAT3{ 260, 0, 0 });
	////enemyGeneratePos.push_back(XMFLOAT3{ 260, 0, 120 });
	std::vector<XMFLOAT3> enemyForwardVec;
	enemyForwardVec.push_back(XMFLOAT3(0, 0, -1));
	enemyForwardVec.push_back(XMFLOAT3(0, 0, 1));
	enemyForwardVec.push_back(XMFLOAT3(1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(-1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(-1, 0, 0));
	enemyForwardVec.push_back(XMFLOAT3(1, 0, 0));

	//EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, cam);

	//画像初期化
	int startGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/start.png");
	int stopGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/stop.png");
	int haniGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/hani.png");
	int particleGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/effect1.png");
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

	bool damaged = false;


	Sprite titleLogo;
	int titleTex = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/TitleGraph.png");
	titleLogo.Init(titleTex);
	XMFLOAT2 titleTexSize = titleLogo.texSize;
	titleLogo.position = { window_width / 2, window_height / 2 , 0.0f };
	titleLogo.size = { window_width, window_height };
	titleLogo.SpriteUpdate();

	int stageNum = 0;

	Vector3 up(0, 0, 1), left(-1, 0, 0);

	Vector3 down = -up;
	Vector3 right = -left;

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

	BombEffect bomb;
#pragma endregion
	//if (FAILED(result))
	//{
	//	return result;
	//}
#pragma endregion
	//描画初期化処理 ここまで
	while (Win->loopBreak()) //ゲームループ
	{
		//ウィンドウメッセージ処理
#pragma region WindowMessage
		Win->msgCheck();
#pragma endregion
		// DirectX毎フレーム処理 ここから
		input->Update();



		//画像処理
		int stopDelay = player.GetStopTimeDelay(); //CT 0~Delay
		int stopCount = player.GetStopTimeCount(); //時間停止 0~60
		bool isStop = player.GetStopTimeFlag(); //停止中か否か

		if (!stopDraw && !isStop && input->KeyTrigger(DIK_RETURN) && stopDelay == STOP_TIME_DELAY)
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
		hani.anchorpoint = { 1.0,1.0 };
		hani.rotation = player.GetAngle() + 90.0f;

		XMFLOAT3 vec3 = player.GetVec3();
		float width = window_width;
		float height = window_height;
		hani.position = { width / 2 ,height / 2 ,0 };

		spriteStart.SpriteUpdate();
		spriteStop.SpriteUpdate();
		hani.SpriteUpdate();



		//更新処理
		if (input->KeyTrigger(DIK_X))
		{
			voice.PlayLoop();
		}
		if (input->KeyTrigger(DIK_Z))
		{
			voice.Stop();
		}
		cam.Update();
		switch (nowScene)
		{
		case TITLE:
			titleLogo.SpriteTransferVertexBuffer();

			if (input->KeyTrigger(DIK_SPACE) || input->ButtonTrigger(XINPUT_GAMEPAD_A))
			{
				nowScene = STAGESELECT;
				cam.Init(XMFLOAT3(0, 250, 0), XMFLOAT3(0, 0, 0), { 0,0,0 }, { 0,0,1 });
				player.Init(cam, STAGE_1);
				EnemyMgr::Instance()->Init(cam);
				//EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, cam);

			}
			break;
		case STAGESELECT:


			if (input->KeyTrigger(DIK_A) || input->KeyTrigger(DIK_D))
			{

				if (input->KeyTrigger(DIK_A))
				{
					stageNum--;
				}
				if (input->KeyTrigger(DIK_D))
				{
					stageNum++;
				}

				if (stageNum < 0)
				{
					stageNum = 0;
				}

				if (stageNum >= 2)
				{
					stageNum = 1;
				}

			}
			if(input->KeyTrigger(DIK_SPACE)|| input->Button(XINPUT_GAMEPAD_A))
			{
				if (stageNum == 0)
				{
					WallMgr::Instance()->Init(loomWalls);
					EnemyMgr::Instance()->Init(cam);
					EnemyMgr::Instance()->Generate(loomEnemyGeneratePos, loomEnemyAngles, cam);
				}
				else if (stageNum == 1)
				{
					stageNum = 1;
					WallMgr::Instance()->Init(townWalls);
					EnemyMgr::Instance()->Init(cam);
					EnemyMgr::Instance()->Generate(townEnemyGeneratePos, townEnemyAngles, cam);
				}
				nowScene = GAME;
			}
			break;
		case GAME:
			XMFLOAT3 moveSpeed = { input->LStick().x , 0.0f, input->LStick().y };

			box.Update(cam);
			XMFLOAT3 enemyPos = { 0,0,50 };
			player.Input(cam);

			//フィールド上の壁
			for (int i = 0; i < WallMgr::Instance()->GetWalls().size(); i++)
			{
				XMFLOAT3 playerSpeed = player.GetVec3();
				XMFLOAT3 push = WallMgr::Instance()->GetWalls()[i].PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);
				playerSpeed = { playerSpeed.x + push.x, playerSpeed.y + push.y ,playerSpeed.z + push.z };
				player.SetVec3(playerSpeed);
			}

			//外周の壁
			for (int i = 0; i < outWall.size(); i++)
			{
				XMFLOAT3 playerSpeed = player.GetVec3();
				XMFLOAT3 push = outWall[i].PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);
				playerSpeed = { playerSpeed.x + push.x, playerSpeed.y + push.y ,playerSpeed.z + push.z };
				player.SetVec3(playerSpeed);

			}

			XMFLOAT3 pos = player.GetPos();

			if (input->KeyTrigger(DIK_RETURN))
			{
				//bomb.Bomb(player.GetPos(), 1);

				int a = 0;
			}

			//敵と自機の押し戻し
			player.PushBack(EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos()));

			player.Update(cam, EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos()));
			bomb.Update();
			player.DeathEffect(cam);
			box.position = enemyPos;
			box.Update(cam);

			box.position = { box.position.x + moveSpeed.x,box.position.y + moveSpeed.y ,box.position.z + moveSpeed.z };

			dome.Update(cam);
			floor.Update(cam);
			{
				Sphere pSphere;
				pSphere.center = XMLoadFloat3(&pos);
				pSphere.radius = 16;
				EnemyMgr::Instance()->Update(player.GetPos(), player.GetAngle(), player.GetStopTimeFlag(), player.GetAttackFlag());
				EnemyMgr::Instance()->UpdateData(cam);

				for (int i = 0; i < EnemyMgr::Instance()->MAX_ENEMY_COUNT; i++) {
					if (EnemyMgr::Instance()->CheckEnemyAttackToPlayer(i, pSphere))
					{
						if (!damaged) player.Damaged();
						damaged = true;
					}
				}
				damaged = false;
			}
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
			}


			//for (int i = 0; i < loomWalls.size(); i++)
			//{
			//	loomWalls[i].Update();
			//}
			WallMgr::Instance()->Update();
			for (int i = 0; i < outWall.size(); i++)
			{
				outWall[i].Update();
			}

			if (player.GetPos().x >= 400)
			{
				nowScene = CLEAR;
			}

			if (player.IsDead())
			{
				nowScene = GAMEOVER;
			}

			break;
		case CLEAR:
			if (input->KeyTrigger(DIK_SPACE))
			{
				nowScene = TITLE;
			}
			break;
		case GAMEOVER:
			if (input->KeyTrigger(DIK_SPACE))
			{
				nowScene = TITLE;
			}
			break;
		default:
			break;
		}
		part.Update();

		//描画
		myDirectX->PreDraw();
		switch (nowScene)
		{
		case TITLE:
			//debugText.Print("", window_width / 2 - 40, window_height / 2, 5);
			titleLogo.SpriteDraw();
			break;

		case STAGESELECT:
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

			hani.SpriteDraw();
			spriteStart.SpriteDraw();
			spriteStop.SpriteDraw();
			break;
		case CLEAR:
			debugText.Print("clear", window_width / 2, window_height / 2, 5);
			break;
		case GAMEOVER:
			debugText.Print("game over", window_width / 2, window_height / 2, 5);
			break;
		default:
			break;
		}

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
