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

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//WindowsAPI初期化処理
#pragma region WindowsAPI

	WinAPI* Win = WinAPI::GetInstance();

	Win->Init(window_width, window_height);
#pragma endregion
#pragma region sound(xAudioInstance)

	Sound::StaticInitialize();
	int alarm = Sound::SoundLoadWave("Resources/Alarm01.wav");

	IXAudio2SourceVoice* voice;
	Sound::CreateSourceVoice(voice, alarm);

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
	cam.Init(XMFLOAT3(0, 50, -90), XMFLOAT3(0, 0, 0), { 0,0,0 }, { 0,0,1 });
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
	wall.Init(cam, { 0.0f,floor.position.y ,0.0f }, { 10, 500, 10 }, { 2.5f, 10, 2.5f });

	//wall
	std::vector<Wall> outWall;
	outWall.resize(4);
	outWall[0].Init(cam, { -(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 500, floor.scale.z }, { 10.0f / 2, 10, floor.scale.z / 2 });
	outWall[1].Init(cam, { 0.0f,floor.position.y ,-(floor.scale.z / 2) }, { floor.scale.x, 10, 10 }, { floor.scale.x / 2 , 10, 10.0f / 2 });
	outWall[2].Init(cam, { +(floor.scale.x / 2),floor.position.y ,0.0f }, { 10, 500, floor.scale.z }, { 10.0f / 2, 10, floor.scale.z / 2 });
	outWall[3].Init(cam, { 0.0f,floor.position.y ,+(floor.scale.z / 2) }, { floor.scale.x, 500, 10 }, { floor.scale.x / 2, 10, 10.0f / 2 });

	EnemyMgr::Instance()->Init(cam);

	int startGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/start.png");
	int stopGH = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/stop.png");

	Sprite spriteStart;
	spriteStart.Init(startGH);

	Sprite spriteStop;
	spriteStop.Init(stopGH);

	spriteStart.position = { window_width / 2,window_height / 2,0 };
	spriteStop.position = { window_width / 2,window_height / 2,0 };

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


		//更新処理
		if (input->Button(XINPUT_GAMEPAD_A))
		{
			Sound::Play(voice, alarm);
		}
		cam.Update();

		XMFLOAT3 moveSpeed = { input->LStick().x , 0.0f, input->LStick().y };

		box.Update(cam);
		XMFLOAT3 enemyPos = { 0,0,50 };
		player.Input(cam);

		XMFLOAT3 playerSpeed = player.GetVec3();
		XMFLOAT3 push = wall.PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);
		playerSpeed = { playerSpeed.x + push.x, playerSpeed.y + push.y ,playerSpeed.z + push.z };
		player.SetVec3(playerSpeed);

		for (int i = 0; i < outWall.size(); i++)
		{
			playerSpeed = player.GetVec3();
			push = outWall[i].PushBack(player.GetPos(), { box.scale.x, 0.0f, box.scale.z }, playerSpeed);
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

		Sphere pSphere;
		pSphere.center = XMLoadFloat3(&pos);
		pSphere.radius = 20;
		EnemyMgr::Instance()->Update(player.GetPos(), pSphere, cam, player.GetStopTimeFlag());
		for (int i = 0; i < EnemyMgr::Instance()->MAX_ENEMY_COUNT; i++) {
			if (EnemyMgr::Instance()->CheckEnemyAttackToPlayer(i, pSphere))
			{
				player.Damaged();
			}
		}
		if (player.IsHit())
		{
			EnemyMgr::Instance()->DeadNearEnemy();
		}
		int hp = player.GetHP();
		bool isdead = player.IsDead();

		wall.Update();

		for (int i = 0; i < outWall.size(); i++)
		{
			outWall[i].Update();
		}
		//描画
		myDirectX->PreDraw();

		if (!player.IsHit()) box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
		floor.modelDraw(boxModel.GetModel(), ModelPipeline::GetInstance()->GetPipeLine());

		player.Draw(model3D->GetPipeLine());
		for (int i = 0; i < outWall.size(); i++)
		{
			outWall[i].Draw();
		}
		box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
		bool isHit = player.IsHit();
		box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
		//dome.modelDraw(domeModel.GetModel(), model3D->GetPipeLine());
		EnemyMgr::Instance()->enemy->enemyBullet.Draw(model3D->GetPipeLine(), boxModel.GetModel());

		EnemyMgr::Instance()->Draw(model3D->GetPipeLine());

		wall.Draw();

		//画像処理
		spriteStart.SpriteUpdate();
		spriteStop.SpriteUpdate();

		spriteStart.SpriteDraw();
		spriteStop.SpriteDraw();


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
