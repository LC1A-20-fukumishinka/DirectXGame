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


//const int spriteSRVCount = 512;


const int window_width = 1280;
const int window_height = 720;

//#pragma region Sound
////チャンクヘッダ
//struct ChunkHeader
//{
//	char id[4];		//チャンク毎のID
//	int32_t size;	//チャンクサイズ
//};
////RIFFヘッダチャンク
//struct RiffHeader
//{
//	ChunkHeader chunk;	//"RIFF"
//	char type[4];		//"WAVE"
//};
////FMTチャンク
//struct FormatChunk
//{
//	ChunkHeader chunk;	//"fmt"
//	WAVEFORMATEX fmt;	//波形フォーマット
//};
////音声データ
//struct SoundData
//{
//	//波形フォーマット
//	WAVEFORMATEX wfex;
//	//バッファの先頭アドレス
//	BYTE *pBuffer;
//	//バッファのサイズ
//	unsigned int bufferSize;
//};
//
////音声データ読み込み
//SoundData SoundLoadWave(const char *filename);
//
////音声データ解放
//void SoundUnload(SoundData *soundData);
//
////音声再生
//void SoundPlayWave(IXAudio2 *xAudio2, const SoundData &soundData);
//#pragma endregion

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//WindowsAPI初期化処理
#pragma region WindowsAPI

	WinAPI *Win = WinAPI::GetInstance();

	Win->Init(window_width, window_height);

	//SpriteCommon spriteCommon;


#pragma endregion

//HRESULT result;

#pragma region sound(xAudioInstance)
	//宣言
	//ComPtr<IXAudio2> xAudio2;
	//IXAudio2MasteringVoice *masterVoice;
	//result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	////マスターボイスを生成
	//result = xAudio2->CreateMasteringVoice(&masterVoice);

	Sound::StaticInitialize();
	Sound alarm;
	alarm.SoundLoadWave("Resources/Alarm01.wav");
#pragma endregion

	//DirectX初期化処理 ここまで
	MyDirectX *myDirectX = MyDirectX::GetInstance();
	//DirectInputの初期化処理ここから

	IGraphicsPipeline *Pipe3D = GraphicsPipeline3D::GetInstance();
	IGraphicsPipeline *model3D = ModelPipeline::GetInstance();
#pragma region DirectInput
		Input * input = Input::GetInstance();

	input->Init(Win->w, Win->hwnd);
#pragma endregion


	//定数バッファ処理
#pragma region ViewMatrix

	Camera cam;
	cam.Init(XMFLOAT3(0, 0, -100), XMFLOAT3(0, 0, 0));
	float angle = 0.0f;	//カメラの回転角

#pragma endregion

	//グラフィックスパイプライン設定
#pragma region gpipeline
#pragma region brend
		//レンダーターゲットのブレンド設定
	//D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	//blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定
//#pragma region NOBLEND
//	blenddesc.BlendEnable = true;					//ブレンドを有効にする
//	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
//	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソース(描画する図形のピクセル)の値を100%使う
//	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デスト(描画対象ピクセル　　　)の値を  0%使う
//#pragma endregion
//
//#pragma region ADD
//	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//加算
//	blenddesc.SrcBlend = D3D12_BLEND_ONE;	//ソースの値を100%使う
//	blenddesc.DestBlend = D3D12_BLEND_ONE;	//デストの値を100%使う
//#pragma endregion
//
//#pragma region SUB
//	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	//加算
//	blenddesc.SrcBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う
//	blenddesc.DestBlend = D3D12_BLEND_ONE;				//デストの値を100%使う
//#pragma endregion
//
//#pragma region INVERSION
//	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
//	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f - デストカラーの値
//	blenddesc.DestBlend = D3D12_BLEND_ZERO;				//使わない
//#pragma endregion
//
//#pragma region ALPHA
//	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
//	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
//	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f - ソースのアルファ値
//#pragma endregion
#pragma endregion
	int spriteTex = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/texture.png");
	Sprite sprite;
	sprite.Init(spriteTex);


	//デバッグテキスト
#pragma region DebugText
	//デバッグテキストのインスタンス
	DebugText debugText;
	////デバッグテキスト用のテクスチャ番号を指定
	//デバッグテクスト用のテクスチャ読み込み

	Object3DCommon obC;


	obC.Init();

	int testTexNum = 0;
	int gridTexNum = 1;

	int test = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/texture.png");
	int grid = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/gridWall.png");
	Model triangle;

	//BillBoard bill;

	//bill.Init(obC, cam, testTexNum);
	//bill.obj.scale = { 10,10,10 };

	Object3D floor[100] = {};


	Easing EQ;
	EQ.Init(60);
	//10の位(行)
	for (int i = 0; i < 10; i++)
	{
		floor[i * 10].Init(obC, cam, gridTexNum);
		floor[i * 10].type = Object3D::Plane;
		//先頭の0番目のオブジェクトが基準になるので飛ばす
		if (i > 0)
		{
			//一つ上のオブジェクトを親に
			floor[i * 10].SetParent(&floor[(i - 1) * 10]);
			//1マス分下にずれる
			floor[i * 10].position = { 0, 2, 0 };
		}
		//1の位(列)
		for (int j = 1; j < 10; j++)
		{
			int num = i * 10 + j;

			floor[num].Init(obC, cam, gridTexNum);
			floor[num].type = Object3D::Plane;
			//一つ左のオブジェクトを親に
			floor[num].SetParent(&floor[num - 1]);
			//1マス分右にずれる
			floor[num].position = { 2, 0, 0 };
		}
	}

	floor[0].scale = { 10,10,10 };
	floor[0].rotation = { 0 , 0, 0 };
	floor[0].position = { -50, -50, 100 };


	Object3D box;
	box.Init(obC, cam, 0);
	box.type = Object3D::Box;
	triangle.CreateModel("skydome");
	box.scale = { 1.0f, 1.0f, 1.0f };
	box.position = { 0,-10,0 };



	XMVECTOR boxQuaternion, quaternion2;

	boxQuaternion = XMQuaternionIdentity();//単位クオータニオンの生成
	quaternion2 = XMQuaternionIdentity();//任意軸(1,0,0)方向に90度回転


	bool isTexture = false;
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

#pragma region DirectX

		input->Update();
		//キー入力の処理方法
		//バックバッファの番号を取得(2つ必要なので0番か1番)


		myDirectX->PreDraw();
		//④描画コマンドここから
		if (input->Key(DIK_SPACE))
		{
			alarm.SoundPlayWave();
		}

#pragma endregion



		//描画ここから

		if (input->Trigger(DIK_RIGHT) || input->Trigger(DIK_LEFT) || input->Trigger(DIK_UP) || input->Trigger(DIK_DOWN))
		{
			if (EQ.IsEnd())
			{
				EQ.Reset();
			}

			quaternion2 = XMQuaternionIdentity();
			if (input->Trigger(DIK_RIGHT))
			{
				box.rotation.y++;
				//quaternion2 = XMQuaternionRotationAxis(XMVECTOR{ 0, 0, 1, 0 }, XMConvertToRadians(-90));//任意軸(0,0,1)方向に90度回転
			}
			if (input->Trigger(DIK_LEFT))
			{
				box.rotation.y--;
				//quaternion2 = XMQuaternionRotationAxis(XMVECTOR{ 0, 0, 1, 0 }, XMConvertToRadians(90));//任意軸(0,0,1)方向に90度回転

			}
			if (input->Trigger(DIK_UP))
			{
				box.rotation.x++;
				//quaternion2 = XMQuaternionRotationAxis(XMVECTOR{ 1, 0, 0, 0 }, XMConvertToRadians(90));//任意軸(0,0,1)方向に90度回転

			}
			if (input->Trigger(DIK_DOWN))
			{
				box.rotation.x--;
				//quaternion2 = XMQuaternionRotationAxis(XMVECTOR{ 1, 0, 0, 0 }, XMConvertToRadians(-90));//任意軸(0,0,1)方向に90度回転
			}
			boxQuaternion = XMQuaternionMultiply(boxQuaternion, quaternion2);
		}


		if (input->Trigger(DIK_1))
		{
			isTexture = !isTexture;
		}
		if (EQ.IsEnd())
		{
			//boxQuaternion = XMQuaternionMultiply(boxQuaternion, quaternion2);
			//box.matWorld = XMMatrixRotationQuaternion(boxQuaternion);
		}
		else
		{
			//float rate = static_cast<float>(EQ.Linear());
			//XMVECTOR tmpQ = XMQuaternionMultiply(boxQuaternion, quaternion2);
			//XMVECTOR rateQ = XMQuaternionSlerp(boxQuaternion, tmpQ, rate);
			//box.matWorld = XMMatrixRotationQuaternion(rateQ);
			//if (EQ.IsEnd())
			//{
			//	//EQ.Reset();
			//	boxQuaternion = XMQuaternionMultiply(boxQuaternion, quaternion2);
			//}

		}
		box.SetConstBuffer(cam);


		for (int i = 0; i < 100; i++)
		{
			floor[i].Update( cam);
		}


		box.Update( cam);

		if (input->Key(DIK_A))
		{
			sprite.position.x--;
		}
		if (input->Key(DIK_D))
		{
			sprite.position.x++;
		}
		sprite.SpriteUpdate();



		//コマンドリストに追加

#pragma region moveCamera
		if (input->Key(DIK_D) || input->Key(DIK_A))
		{
			if (input->Key(DIK_D))
			{
				angle += XMConvertToRadians(1.0f);
			}
			else if (input->Key(DIK_A))
			{
				angle -= XMConvertToRadians(1.0f);
			}
			cam.eye.x = -100 * sinf(angle);
			cam.eye.z = -100 * cosf(angle);
		}

		cam.Update();
#pragma endregion

		//描画コマンド

		//sprite.SpriteDraw();


		//debugText.Print("Hello,DirectX!!", 200, 100);

		//debugText.Print("abcdefghijklmnopqrstuvwxyz", 200, 200, 2.0f);

		//debugText.DrawAll();

		//深度地リセット
		DepthReset();
		//bill.Draw(obC);
		//box.Draw(obC, Pipe3D->GetPipeLine(), test);

		box.modelDraw(triangle.GetModel(), model3D->GetPipeLine(), isTexture, test);
		//for (int i = 0; i < 100; i++)
		//{
		//	floor[i].Draw(obC, Pipe3D->GetPipeLine(), grid);
		//}

		//④描画コマンドここまで

	//⑤リソースバリアを戻す
		myDirectX->PostDraw();
		// DirectX毎フレーム処理 ここまで
	}
	//xAudio2解放
	Sound::xAudioDelete();
	//音声データ解放
	alarm.SoundUnload();
	//どこでメモリリークを起こしているかを確認してくれた
	//ID3D12DebugDevice *debugInterface;

	//if (SUCCEEDED(dev.Get()->QueryInterface(&debugInterface)))
	//{
	//	debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
	//	debugInterface->Release();
	//}
	//ウィンドウクラスを登録解除
#pragma region WindowsAPI
	Win->end();
#pragma endregion
	return 0;
}


//SoundData SoundLoadWave(const char *filename)
//{
//	//HRESULT result;
//#pragma region fileopen
//	//ファイル入力ストリームのインスタンス
//	std::ifstream file;
//	//wavファイルをバイナリモードで開く
//	file.open(filename, std::ios_base::binary);
//	//ファイルオープン失敗を検出する
//	assert(file.is_open());
//#pragma endregion
//#pragma region wavRead
//	//RIFFヘッダーの読み込み
//	RiffHeader riff;
//	file.read((char *)&riff, sizeof(riff));
//	//ファイルがRiffかチェック
//	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
//	{
//		assert(0);
//	}
//	//たイペがWAVEかチェック
//	if (strncmp(riff.type, "WAVE", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//formatチャンクの読み込み
//	FormatChunk format = {};
//	//チャンクヘッダの確認
//	file.read((char *)&format, sizeof(ChunkHeader));
//	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//チャンク本体の読み込み
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char *)&format.fmt, format.chunk.size);
//
//	//Dataチャンクの読み込み
//	ChunkHeader data;
//	file.read((char *)&data, sizeof(data));
//	//JUNKチャンクを検出した場合
//	if (strncmp(data.id, "JUNK", 4) == 0)
//	{
//		//読み取り位置をJUNKチャンクの終わりまで進める
//		file.seekg(data.size, std::ios_base::cur);
//		//再読み込み
//		file.read((char *)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id, "data", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//Dataチャンクのデータ部(波形データ)の読み込み
//	char *pBuffer = new char[data.size];
//	file.read(pBuffer, data.size);
//
//
//#pragma endregion
//#pragma region fileClose
//	//WAVEファイルを閉じる
//	file.close();
//#pragma endregion
//#pragma region makeSound
//	//returnする為のおんせいデータ
//	SoundData soundData = {};
//
//	soundData.wfex = format.fmt;
//	soundData.pBuffer = reinterpret_cast<BYTE *>(pBuffer);
//	soundData.bufferSize = data.size;
//#pragma endregion
//
//	return soundData;
//}
//
//void SoundUnload(SoundData *soundData)
//{
//	//バッファのメモリを解放
//	delete[] soundData->pBuffer;
//
//	soundData->pBuffer = 0;
//	soundData->bufferSize = 0;
//	soundData->wfex = {};
//}
//
//void SoundPlayWave(IXAudio2 *xAudio2, const SoundData &soundData)
//{
//	{
//		HRESULT result;
//
//		//波形フォーマットをもとにSourceVolでの生成
//		IXAudio2SourceVoice *pSourceVoice = nullptr;
//		result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
//		assert(SUCCEEDED(result));
//
//		//再生する波形データの設定
//		XAUDIO2_BUFFER buf{};
//		buf.pAudioData = soundData.pBuffer;
//		buf.AudioBytes = soundData.bufferSize;
//		buf.Flags = XAUDIO2_END_OF_STREAM;
//
//		//波形データの再生
//		result = pSourceVoice->SubmitSourceBuffer(&buf);
//		result = pSourceVoice->Start();
//	}
//
//}

