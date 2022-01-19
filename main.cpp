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

struct Vertex
{
	XMFLOAT3 pos;	//xyz座標
	XMFLOAT3 normal;
	XMFLOAT2 uv;	//uv座標
};

struct ConstBufferData
{
	XMFLOAT4 color;	//色(RGBA)
	XMMATRIX mat;	//3D変換行列
};

const int window_width = 1280;
const int window_height = 720;

enum Scenes
{
	title,
	game,
	clear
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

	IXAudio2SourceVoice *voice;
	Sound::CreateSourceVoice(voice, alarm);

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
	cam.Init(XMFLOAT3(-450, 250, 0), XMFLOAT3(-350, 0, 0), { 0,0,0 }, { 0,0,1 });
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
	player.Init(cam);



	Object3D floor;
	floor.scale = { 1000.0f, 2.0f, 300.0f };
	floor.position = { 0.0f, -20.0f, 0.0f };
	floor.color = { 3.0f,2.0f ,2.0f ,1.0f };
	floor.Init(cam);


	std::vector<Wall> walls;
	Wall wall;
	DirectX::XMFLOAT3 pillarScale = { 10, 500, 10 };
	DirectX::XMFLOAT3 LPillarScale = { 50, 500, 50 };
	DirectX::XMFLOAT3 wideWallScale = { 200, 500, 10 };
	DirectX::XMFLOAT3 heightWallScale = { 10, 500, 100 };
	DirectX::XMFLOAT3 LHeightWallScale = { 80, 500, 150 };
	DirectX::XMFLOAT3 LWidthWallScale = { 150, 500, 100 };


	wall.Init(cam, { 0.0f,floor.position.y ,0.0f }, pillarScale, { pillarScale.x / 4, 10, pillarScale.z / 4 });
	walls.push_back(wall);

	//入口奥
	wall.Init(cam, { -floor.scale.x / 2 + 100,floor.position.y ,floor.scale.z / 2 - 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	walls.push_back(wall);
	wall.Init(cam, { -floor.scale.x / 2 + 200,floor.position.y ,floor.scale.z / 2 - 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	walls.push_back(wall);

	//入口手前
	wall.Init(cam, { -floor.scale.x / 2 + 100,floor.position.y ,-floor.scale.z / 2 + 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	walls.push_back(wall);
	wall.Init(cam, { -floor.scale.x / 2 + 200,floor.position.y ,-floor.scale.z / 2 + 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	walls.push_back(wall);

	//入り口側中央柱
	wall.Init(cam, { -180,floor.position.y ,0 }, LHeightWallScale, { LHeightWallScale.x / 2, 10, LHeightWallScale.z / 2 });
	walls.push_back(wall);

	//出口側二本柱
	wall.Init(cam, { +200,floor.position.y ,-80 }, LPillarScale, { LPillarScale.x / 2, 10, LPillarScale.z / 2 });
	walls.push_back(wall);
	wall.Init(cam, { +200,floor.position.y ,+80 }, LPillarScale, { LPillarScale.x / 2, 10, LPillarScale.z / 2 });
	walls.push_back(wall);

	//中間通路壁
	wall.Init(cam, { 0,floor.position.y ,floor.scale.z / 2 - 50 }, LWidthWallScale, { LWidthWallScale.x / 2, 10, LWidthWallScale.z / 2 });
	walls.push_back(wall);
	wall.Init(cam, { 0,floor.position.y ,-floor.scale.z / 2 + 50 }, LWidthWallScale, { LWidthWallScale.x / 2, 10, LWidthWallScale.z / 2 });
	walls.push_back(wall);

	//出口奥
	wall.Init(cam, { floor.scale.x / 2 - 100,floor.position.y ,floor.scale.z / 2 - 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	walls.push_back(wall);
	wall.Init(cam, { floor.scale.x / 2 - 200,floor.position.y ,floor.scale.z / 2 - 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	walls.push_back(wall);

	//出口手前
	wall.Init(cam, { +floor.scale.x / 2 - 100,floor.position.y ,-floor.scale.z / 2 + 100 }, wideWallScale, { wideWallScale.x / 2, 10, wideWallScale.z / 2 });
	walls.push_back(wall);
	wall.Init(cam, { +floor.scale.x / 2 - 200,floor.position.y ,-floor.scale.z / 2 + 50 }, heightWallScale, { heightWallScale.x / 2, 10, heightWallScale.z / 2 });
	walls.push_back(wall);
	//wall
	std::vector<Wall> outWall;
	outWall.resize(4);
	outWall[0].Init(cam, { -(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 500, floor.scale.z }, { 10.0f / 2, 10, floor.scale.z / 2 });
	outWall[1].Init(cam, { 0.0f,floor.position.y ,-(floor.scale.z / 2) }, { floor.scale.x, 10, 10 }, { floor.scale.x / 2 , 10, 10.0f / 2 });
	outWall[2].Init(cam, { +(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 500, floor.scale.z }, { 10.0f / 2, 10, floor.scale.z / 2 });
	outWall[3].Init(cam, { 0.0f,floor.position.y ,+(floor.scale.z / 2) }, { floor.scale.x, 500, 10 }, { floor.scale.x / 2, 10, 10.0f / 2 });

	EnemyMgr::Instance()->Init(cam);

	Scenes nowScene = title;
	std::vector<XMFLOAT3> enemyGeneratePos;
	enemyGeneratePos.push_back(XMFLOAT3{ -255, 0, 105 });
	enemyGeneratePos.push_back(XMFLOAT3{ -255, 0, -105 });
	enemyGeneratePos.push_back(XMFLOAT3{ -105, 0, 0 });
	enemyGeneratePos.push_back(XMFLOAT3{ 45, 0, 30 });
	enemyGeneratePos.push_back(XMFLOAT3{ 45, 0, -30 });
	enemyGeneratePos.push_back(XMFLOAT3{ 130, 0, 0 });
	enemyGeneratePos.push_back(XMFLOAT3{ 260, 0, -120 });
	enemyGeneratePos.push_back(XMFLOAT3{ 260, 0, 0 });
	enemyGeneratePos.push_back(XMFLOAT3{ 260, 0, 120 });

	EnemyMgr::Instance()->Generate(enemyGeneratePos, cam);
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
		if (input->KeyTrigger(DIK_RETURN))
		{
			int a = 0;
		}

		//更新処理
		if (input->Button(XINPUT_GAMEPAD_A))
		{
			Sound::Play(voice, alarm);
		}

		cam.Update();
		switch (nowScene)
		{
		case title:
			if (input->KeyTrigger(DIK_SPACE))
			{
				nowScene = game;
				cam.Init(XMFLOAT3(-450, 250, 0), XMFLOAT3(-350, 0, 0), { 0,0,0 }, { 0,0,1 });
				player.Init(cam);
				EnemyMgr::Instance()->Generate(enemyGeneratePos, cam);

			}
			break;
		case game:
			XMFLOAT3 moveSpeed = { input->LStick().x , 0.0f, input->LStick().y };

			box.Update(cam);
			XMFLOAT3 enemyPos = { 0,0,50 };
			player.Input(cam);

			for (int i = 0; i < walls.size(); i++)
			{
				XMFLOAT3 playerSpeed = player.GetVec3();
				XMFLOAT3 push = walls[i].PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);
				playerSpeed = { playerSpeed.x + push.x, playerSpeed.y + push.y ,playerSpeed.z + push.z };
				player.SetVec3(playerSpeed);
			}
			for (int i = 0; i < outWall.size(); i++)
			{
				XMFLOAT3 playerSpeed = player.GetVec3();
				XMFLOAT3 push = outWall[i].PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);
				playerSpeed = { playerSpeed.x + push.x, playerSpeed.y + push.y ,playerSpeed.z + push.z };
				player.SetVec3(playerSpeed);

			}
			XMFLOAT3 pos = player.GetPos();
			player.Update(cam, EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos()));
			box.position = enemyPos;
			box.Update(cam);

			box.position = { box.position.x + moveSpeed.x,box.position.y + moveSpeed.y ,box.position.z + moveSpeed.z };

			dome.Update(cam);
			floor.Update(cam);

			{
				Sphere pSphere;
				pSphere.center = XMLoadFloat3(&pos);
				pSphere.radius = 20;
				EnemyMgr::Instance()->Update(player.GetPos(), pSphere, cam);
				for (int i = 0; i < EnemyMgr::Instance()->MAX_ENEMY_COUNT; i++) {
					if (EnemyMgr::Instance()->CheckEnemyAttackToPlayer(i, pSphere))
					{
						player.Damaged();
					}
				}
			}
			if (player.IsHit())
			{
				EnemyMgr::Instance()->DeadNearEnemy();
			}
			{int hp = player.GetHP(); }

			{bool isdead = player.IsDead(); }
			for (int i = 0; i < walls.size(); i++)
			{
				walls[i].Update();
			}
			for (int i = 0; i < outWall.size(); i++)
			{
				outWall[i].Update();
			}

			if (player.GetPos().x >= 480)
			{
				nowScene = clear;
			}
			break;
		case clear:
			if (input->KeyTrigger(DIK_SPACE))
			{
				nowScene = title;
			}
			break;
		default:
			break;
		}
		//描画
		myDirectX->PreDraw();
		switch (nowScene)
		{
		case title:
		debugText.Print("title", window_width/2-40, window_height / 2, 5);
			break;
		case game:
			if (!player.IsHit()) box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			floor.modelDraw(boxModel.GetModel(), ModelPipeline::GetInstance()->GetPipeLine());

			player.Draw(model3D->GetPipeLine());
			for (int i = 0; i < outWall.size(); i++)
			{
				outWall[i].Draw();
			}
			box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			{bool isHit = player.IsHit();}
			box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
			//dome.modelDraw(domeModel.GetModel(), model3D->GetPipeLine());

			EnemyMgr::Instance()->Draw(model3D->GetPipeLine());

			for (int i = 0; i < walls.size(); i++)
			{
				walls[i].Draw();
			}
			break;
		case clear:
			debugText.Print("clear", window_width / 2 - 40, window_height / 2, 5);
			break;
		default:
			break;
		}

		//深度地リセット
		DepthReset();


		//.Print("Hello,DirectX!!", 200, 100);

		//debugText.Print("abcdefghijklmnopqrstuvwxyz", 200, 200, 2.0f);
		debugText.DrawAll();
		//�C描画コマンドここまで

	//�Dリソースバリアを戻す
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
