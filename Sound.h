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


public:
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
	static void CreateSourceVoice(IXAudio2SourceVoice *&pSourceVoice, const int soundIndex);

public://静的メンバ変数

	static Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	static IXAudio2MasteringVoice *masterVoice;



public://メンバ変数
	//音声データ読み込み
	static int SoundLoadWave(const char *filename);
	//音声データ解放
	static void SoundUnload();

	//音声再生
	static void Play(IXAudio2SourceVoice *pSourceVoice, const int soundIndex);

	static void PlayLoop(IXAudio2SourceVoice *pSourceVoice, const int soundIndex);

	static void Stop(IXAudio2SourceVoice *pSourceVoice);
private:
	static std::vector<SoundData> soundData;
};