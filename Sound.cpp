#include "Sound.h"
#include "d3dx12.h"
#include <cassert>
#include "SafeDelete.h"

Microsoft::WRL::ComPtr<IXAudio2> Sound::xAudio2;
IXAudio2MasteringVoice *Sound::masterVoice;

bool Sound::StaticInitialize()
{
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
}

void Sound::xAudioDelete()
{
	xAudio2.Reset();
}

Sound::SoundData Sound::SoundLoadWave(const char *filename)
{
	//HRESULT result;
#pragma region fileopen
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	
	(file.is_open());
#pragma endregion
#pragma region wavRead
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char *)&riff, sizeof(riff));
	//ファイルがRiffかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//たイペがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダの確認
	file.read((char *)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char *)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char *)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char *)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char *pBuffer = new char[data.size];
	file.read(pBuffer, data.size);


#pragma endregion
#pragma region fileClose
	//WAVEファイルを閉じる
	file.close();
#pragma endregion
#pragma region makeSound
	//returnする為のおんせいデータ
	SoundData soundData = {};

	this->data.wfex = format.fmt;
	this->data.pBuffer = reinterpret_cast<BYTE *>(pBuffer);
	this->data.bufferSize = data.size;
#pragma endregion

	return soundData;
}

void Sound::SoundUnload()
{
	//バッファのメモリを解放
	delete[] data.pBuffer;

	data.pBuffer = 0;
	data.bufferSize = 0;
	data.wfex = {};
}

void Sound::SoundPlayWave()
{
	{
		HRESULT result;

		//波形フォーマットをもとにSourceVolでの生成
		IXAudio2SourceVoice *pSourceVoice = nullptr;
		result = xAudio2->CreateSourceVoice(&pSourceVoice, &data.wfex);
		assert(SUCCEEDED(result));

		//再生する波形データの設定
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = data.pBuffer;
		buf.AudioBytes = data.bufferSize;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		//波形データの再生
		result = pSourceVoice->SubmitSourceBuffer(&buf);
		result = pSourceVoice->Start();
	}

}

