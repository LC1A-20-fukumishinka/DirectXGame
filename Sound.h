#pragma once
#include <xaudio2.h>
#include <fstream>
#include <d3dx12.h>

#pragma comment(lib, "xaudio2.lib")


#pragma endregion
class Sound
{
private://�T�u�N���X
#pragma region Sound

public:
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];		//�`�����N����ID
		int32_t size;	//�`�����N�T�C�Y
	};
	//RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};
	//FMT�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMATEX fmt;	//�g�`�t�H�[�}�b�g
	};
	//�����f�[�^
	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		BYTE *pBuffer;
		//�o�b�t�@�̃T�C�Y
		unsigned int bufferSize;
	};
public://�ÓI�����o�֐�
	static bool StaticInitialize();
	static void xAudioDelete();
	static void CreateSourceVoice(IXAudio2SourceVoice *&pSourceVoice, const int soundIndex);
	//�����f�[�^�ǂݍ���
	static int SoundLoadWave(const char *filename);
	//�����f�[�^���
	static void SoundUnload();


public://�ÓI�����o�ϐ�

	static Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	static IXAudio2MasteringVoice *masterVoice;
private:
	static std::vector<SoundData> soundData;


public://�����o�֐�
	Sound(const int soundIndex);

	//�����Đ�
	void Play();

	void PlayLoop();

	void Stop();
private://�����o�ϐ�
	IXAudio2SourceVoice *pSourceVoice;
	int soundIndex;
};