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

	WinAPI *Win = WinAPI::GetInstance();

	Win->Init(window_width, window_height);
#pragma endregion
#pragma region sound(xAudioInstance)

	Sound::StaticInitialize();
	int alarm = Sound::SoundLoadWave("Resources/Alarm01.wav");

	IXAudio2SourceVoice *voice;
	Sound::CreateSourceVoice(voice, alarm);

#pragma endregion

	//DirectX���������� �����܂�
	MyDirectX *myDirectX = MyDirectX::GetInstance();
	//DirectInput�̏�����������������

	IGraphicsPipeline *Pipe3D = GraphicsPipeline3D::GetInstance();
	IGraphicsPipeline *model3D = ModelPipeline::GetInstance();
#pragma region DirectInput
	Input *input = Input::GetInstance();

	input->Init(Win->w, Win->hwnd);
#pragma endregion

	Camera cam;
	cam.Init(XMFLOAT3(0, 0, -100), XMFLOAT3(0, 0, 0));
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
	box.position = { -10,0,0 };
	box.Init(cam);

	Object3D dome;
	dome.Init(cam);
	dome.scale = { 3.0f, 3.0f, 3.0f };
	dome.position = { 10, 0,0 };
	bool isTexture = false;
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
		if (input->Button(XINPUT_GAMEPAD_A) ||input->Key(DIK_A))
		{
			Sound::Play(voice, alarm);
		}
		cam.Update();

		box.Update(cam);

		dome.Update(cam);

		//�`��
		myDirectX->PreDraw();

		box.modelDraw(boxModel.GetModel(), model3D->GetPipeLine());
		dome.modelDraw(domeModel.GetModel(), model3D->GetPipeLine());

		//�[�x�n���Z�b�g
		DepthReset();


		debugText.Print("Hello,DirectX!!", 200, 100);

		debugText.Print("abcdefghijklmnopqrstuvwxyz", 200, 200, 2.0f);
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
