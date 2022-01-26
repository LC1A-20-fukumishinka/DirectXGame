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
	//�}�X�^�[�{�C�X�𐶐�
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
	//�g�`�t�H�[�}�b�g�����Ƃ�SourceVol�ł̐���
	pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[soundIndex].wfex);
	assert(SUCCEEDED(result));

}

int Sound::SoundLoadWave(const char *filename)
{
	//HRESULT result;
#pragma region fileopen
	//�t�@�C�����̓X�g���[���̃C���X�^���X
	std::ifstream file;
	//wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(filename, std::ios_base::binary);
	//�t�@�C���I�[�v�����s�����o����

	(file.is_open());
#pragma endregion
#pragma region wavRead
	//RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char *)&riff, sizeof(riff));
	//�t�@�C����Riff���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//���C�y��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//format�`�����N�̓ǂݍ���
	FormatChunk format = {};
	//�`�����N�w�b�_�̊m�F
	file.read((char *)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}

	//�`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char *)&format.fmt, format.chunk.size);

	//Data�`�����N�̓ǂݍ���
	ChunkHeader chunkData;
	file.read((char *)&chunkData, sizeof(chunkData));
	//JUNK�`�����N�����o�����ꍇ
	if (strncmp(chunkData.id, "JUNK", 4) == 0)
	{
		//�ǂݎ��ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(chunkData.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char *)&chunkData, sizeof(chunkData));
	}

	if (strncmp(chunkData.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Data�`�����N�̃f�[�^��(�g�`�f�[�^)�̓ǂݍ���
	char *pBuffer = new char[chunkData.size];
	file.read(pBuffer, chunkData.size);


#pragma endregion
#pragma region fileClose
	//WAVE�t�@�C�������
	file.close();
#pragma endregion
#pragma region makeSound
	//return����ׂ̂��񂹂��f�[�^
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
	//�o�b�t�@�̃����������
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

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData[soundIndex].pBuffer;
	buf.AudioBytes = soundData[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = 0;

	//�g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

void Sound::PlayLoop()
{
	HRESULT result;

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData[soundIndex].pBuffer;
	buf.AudioBytes = soundData[soundIndex].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//�g�`�f�[�^�̍Đ�
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
	//�g�`�t�H�[�}�b�g�����Ƃ�SourceVol�ł̐���
	pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[this->soundIndex].wfex);
	assert(SUCCEEDED(result));
}

