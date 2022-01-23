#include "Sound.h"
#include "d3dx12.h"
#include <cassert>
#include "SafeDelete.h"

Microsoft::WRL::ComPtr<IXAudio2> Sound::xAudio2;
IXAudio2MasteringVoice *Sound::masterVoice;
std::vector<Sound::SoundData> Sound::soundData;

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

void Sound::CreateSourceVoice(IXAudio2SourceVoice *&pSourceVoice, const int soundIndex)
{
	HRESULT result;
	//波形フォーマットをもとにSourceVolでの生成
	pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[soundIndex].wfex);
	assert(SUCCEEDED(result));

}

int Sound::SoundLoadWave(const char *filename)
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
	ChunkHeader chunkData;
	file.read((char *)&chunkData, sizeof(chunkData));
	//JUNKチャンクを検出した場合
	if (strncmp(chunkData.id, "JUNK", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(chunkData.size, std::ios_base::cur);
		//再読み込み
		file.read((char *)&chunkData, sizeof(chunkData));
	}

	if (strncmp(chunkData.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char *pBuffer = new char[chunkData.size];
	file.read(pBuffer, chunkData.size);


#pragma endregion
#pragma region fileClose
	//WAVEファイルを閉じる
	file.close();
#pragma endregion
#pragma region makeSound
	//returnする為のおんせいデータ
	SoundData newData = {};

	newData.wfex = format.fmt;
	newData.pBuffer = reinterpret_cast<BYTE *>(pBuffer);
	newData.bufferSize = chunkData.size;
#pragma endregion

	int dataNumber = soundData.size();
	soundData.push_back(newData);
	return dataNumber;
}

void Sound::SoundUnload()
{
	//バッファのメモリを解放
	for (int i = 0; i < soundData.size(); i++)
	{
		delete[] soundData[i].pBuffer;

		soundData[i].pBuffer = 0;
		soundData[i].bufferSize = 0;
		soundData[i].wfex = {};
	}
}

void Sound::Play()
{
	HRESULT result;

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData[soundIndex].pBuffer;
	buf.AudioBytes = soundData[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = 0;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

void Sound::PlayLoop()
{
	HRESULT result;

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData[soundIndex].pBuffer;
	buf.AudioBytes = soundData[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

void Sound::Stop()
{
	pSourceVoice->Stop();
	pSourceVoice->FlushSourceBuffers();
}

Sound::Sound(const int soundIndex)
{
	this->soundIndex = soundIndex;

	HRESULT result;
	//波形フォーマットをもとにSourceVolでの生成
	pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[this->soundIndex].wfex);
	assert(SUCCEEDED(result));
}

