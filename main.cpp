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
//#include "EnemyMgr.h"
#include "Wall.h"
#include "EnemyMgr.h"
#include "Player.h"

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xaudio2.lib")

/*
	�f�G�ȃR�����g
	aaaaaaaaaa
	�撣���Ă������I
	����ɂ��́I
*/

struct Vertex
{
	XMFLOAT3 pos;	//xyz���W
	XMFLOAT3 normal;
	XMFLOAT2 uv;	//uv���W
};

struct ConstBufferData
{
	XMFLOAT4 color;	//�F(RGBA)
	XMMATRIX mat;	//3D�ϊ��s��
};

const int window_width = 1280;
const int window_height = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//WindowsAPI����������
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

	//DirectX���������� �����܂�
	MyDirectX* myDirectX = MyDirectX::GetInstance();
	//DirectInput�̏�����������������

	IGraphicsPipeline* Pipe3D = GraphicsPipeline3D::GetInstance();
	IGraphicsPipeline* model3D = ModelPipeline::GetInstance();
#pragma region DirectInput
	Input* input = Input::GetInstance();

	input->Init(Win->w, Win->hwnd);
#pragma endregion


	Camera cam;
	cam.Init(XMFLOAT3(0, 50, -90), XMFLOAT3(0, 0, 0), { 0,0,0 }, { 0,0,1 });
	float angle = 0.0f;	//�J�����̉�]�p

	DebugText debugText;

#pragma region gpipeline
#pragma region DebugText


	//���f������
	Model boxModel;
	Model domeModel;
	//���f���̃t�@�C���ǂݍ���
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

	//��
	Sphere sphere;
	sphere.center = { 20,10,0 };
	sphere.radius = 20.0f;

	Wall::SetModel(boxModel);
	Wall wall;
	wall.Init(cam, { 0.0f,0.0f ,0.0f }, { 10, 10, 10 }, { 2.5f, 10, 2.5f });
	Player player;
	player.Init(cam);

	EnemyMgr::Instance()->Init(cam);

#pragma endregion
	//if (FAILED(result))
	//{
	//	return result;
	//}
#pragma endregion
	//�`�揉�������� �����܂�
	while (Win->loopBreak()) //�Q�[�����[�v
	{
		//�E�B���h�E���b�Z�[�W����
#pragma region WindowMessage
		Win->msgCheck();
#pragma endregion
		// DirectX���t���[������ ��������
		input->Update();


		//�X�V����
		if (input->Button(XINPUT_GAMEPAD_A))
		{
			Sound::Play(voice, alarm);
		}
		cam.Update();

		XMFLOAT3 moveSpeed = { input->LStick().x , 0.0f, input->LStick().y };

		XMFLOAT3 push = wall.PushBack(box.position, { box.scale.x / 4, 0.0f, box.scale.z / 4 }, moveSpeed);
		moveSpeed = { push.x + moveSpeed.x,push.y + moveSpeed.y ,push.z + moveSpeed.z };
		box.Update(cam);
		XMFLOAT3 enemyPos = { 0,0,50 };
		player.Input(cam);
		XMFLOAT3 pos = player.GetPos();
		player.Update(cam, EnemyMgr::Instance()->GetNearEnemyPos(player.GetPos()));
		box.position = enemyPos;
		box.Update(cam);

		box.position = { box.position.x + moveSpeed.x,box.position.y + moveSpeed.y ,box.position.z + moveSpeed.z };

		dome.Update(cam);
		wall.Update();

		Sphere pSphere;
		XMFLOAT3 pos2 = player.GetPos();
		pSphere.center = XMLoadFloat3(&pos);
		pSphere.radius = 20;
		EnemyMgr::Instance()->Update(player.GetPos(), pSphere, cam);
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

		//�`��
		myDirectX->PreDraw();

		//bool isHit = player.isHit;
		box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
		//dome.modelDraw(domeModel.GetModel(), model3D->GetPipeLine());

		EnemyMgr::Instance()->Draw(model3D->GetPipeLine());

		wall.Draw();
		player.Draw(model3D->GetPipeLine());

		//�[�x�n���Z�b�g
		DepthReset();


		//.Print("Hello,DirectX!!", 200, 100);

		//debugText.Print("abcdefghijklmnopqrstuvwxyz", 200, 200, 2.0f);
		debugText.DrawAll();
		//�C�`��R�}���h�����܂�

	//�D���\�[�X�o���A��߂�
		myDirectX->PostDraw();
		// DirectX���t���[������ �����܂�
	}
	//xAudio2���
	Sound::xAudioDelete();
	//�E�B���h�E�N���X��o�^����
#pragma region WindowsAPI
	Win->end();
#pragma endregion
	return 0;
}
