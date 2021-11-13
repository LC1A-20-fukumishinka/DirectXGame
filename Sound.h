#pragma once
#include <xaudio2.h>
#include <fstream>
#include <d3dx12.h>

#pragma comment(lib, "xaudio2.lib")


#pragma endregion
class Sound
{
private://サブクラス
#pragma region Sound
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];		//チャンク毎のID
		int32_t size;	//チャンクサイズ
	};
	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};
	//FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMATEX fmt;	//波形フォーマット
	};
	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE *pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;
	};


public://静的メンバ関数
	static bool StaticInitialize();
	static void xAudioDelete();


public://静的メンバ変数

	static Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	static IXAudio2MasteringVoice *masterVoice;



public://メンバ変数
	//音声データ読み込み
	SoundData SoundLoadWave(const char *filename);
	//音声データ解放
	void SoundUnload();

	//音声再生
	void SoundPlayWave();
private:
	SoundData data;
};