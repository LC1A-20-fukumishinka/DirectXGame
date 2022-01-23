#include "Model.h"
#include <d3dx12.h>
#include "MyDirectX.h"
#include <fstream>//�t�@�C�����o��
#include <sstream>//������ɑ΂�����o��
#include <string>//������
#include <vector>//���_�f�[�^��Z�߂�
#include "TextureMgr.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
Model::Model()
{
	model.textureHandle = -1;
}

Model::~Model()
{
}


void Model::CreateModel(const std::string &modelname)
{
	const string directoryPath = "Resources/" + modelname + "/";	//�hResouces/triangle_mat/�h

	LoadModel(directoryPath, modelname);

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * model.vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * model.indices.size());

	ID3D12Device *device = MyDirectX::GetInstance()->GetDevice();
	// ���_�o�b�t�@����
	HRESULT result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		//&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.vertBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUv *vertMap = nullptr;
	result = model.vertBuff->Map(0, nullptr, (void **)&vertMap);
	if (SUCCEEDED(result)) {
		//memcpy(vertMap, vertices, sizeof(vertices));
		std::copy(model.vertices.begin(), model.vertices.end(), vertMap);
		model.vertBuff->Unmap(0, nullptr);
	}


	// �C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		//&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices)),
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.indexBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short *indexMap = nullptr;
	result = model.indexBuff->Map(0, nullptr, (void **)&indexMap);
	if (SUCCEEDED(result)) {

		// �S�C���f�b�N�X�ɑ΂���
		//for (int i = 0; i < _countof(indices); i++)
		//{
		//	indexMap[i] = indices[i];	// �C���f�b�N�X���R�s�[
		//}
		std::copy(model.indices.begin(), model.indices.end(), indexMap);
		model.indexBuff->Unmap(0, nullptr);
	}



	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.constBuffB1));

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataB1 *constMap = nullptr;
	result = model.constBuffB1->Map(0, nullptr, (void **)&constMap);
	constMap->ambient = model.material.ambient;
	constMap->diffuse = model.material.diffuse;
	constMap->specular = model.material.specular;
	constMap->alpha = model.material.alpha;
	model.constBuffB1->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̍쐬
	model.vbView.BufferLocation = model.vertBuff->GetGPUVirtualAddress();
	//vbView.SizeInBytes = sizeof(vertices);
	model.vbView.SizeInBytes = sizeVB;
	model.vbView.StrideInBytes = sizeof(model.vertices[0]);

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	model.ibView.BufferLocation = model.indexBuff->GetGPUVirtualAddress();
	model.ibView.Format = DXGI_FORMAT_R16_UINT;
	//ibView.SizeInBytes = sizeof(indices);
	model.ibView.SizeInBytes = sizeIB;
}


void Model::LoadModel(const std::string &directoryPath, const std::string &modelname)
{

	//�t�@�C���X�g���[��
	std::ifstream file;
	////.obj�t�@�C�����J��
	//file.open("Resources/triangle_tex/triangle_tex.obj");
	const string filename = modelname + ".obj";	//�htriangle_mat.obj�h
	file.open(directoryPath + filename);	//�hResouces/triangle_mat/triangle_mat.obj�h

	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}

	vector<XMFLOAT3> positions;		//���_���
	vector<XMFLOAT3> normals;		//�@���x�N�g��
	vector<XMFLOAT2> texcoords;		//�e�N�X�`��UV
	//1�s���ǂݍ���
	string line;
	while (getline(file, line))
	{
		//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������4
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�o�s�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�擪������v�Ȃ璸�_���W
		if (key == "v")
		{
			//X,Y,Z���W�ǂݍ���
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//���W�f�[�^�ɒǉ�
			positions.emplace_back(position);
			////���_�f�[�^�ɒǉ�
			//VertexPosNormalUv vertex{};
			//vertex.pos = position;
			//vertices.emplace_back(vertex);
		}

		//�擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			//U,V�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V�������]
			texcoord.y = 1.0f - texcoord.y;
			//�e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}

		//�擪������vn�Ȃ�@���x�N�g��
		if (key == "vn")
		{
			//X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//�@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}

		//�擪������f�Ȃ�|���S�� (�O�p�`)
		if (key == "f")
		{
			//���p�X�y�[�X��؂�o�s�̑�����ǂݍ���
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//���_�C���f�b�N�X1�����̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);//�X���b�V�����΂�
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);//�X���b�V�����΂�
				index_stream >> indexNormal;

				VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				model.vertices.emplace_back(vertex);
				//���_�C���f�b�N�X�ɒǉ�
				model.indices.emplace_back((unsigned short)model.indices.size());

				//const int squareSurfaceVerticesCount = 4;
				////�ʂ̒��_����4
				//if (vertices.size() >= squareSurfaceVerticesCount)
				//{
				//	const size_t squareIndexstartValue = 4;
				//	indices.emplace_back((unsigned short)indices.size() - squareIndexstartValue);

				//	const size_t squareIndexBackValue = 2;
				//	indices.emplace_back((unsigned short)indices.size() - squareIndexBackValue);
				//}

			}
		}

		if (key == "mtllib")
		{
			//�}�e���A���̃t�@�C���̓ǂݍ���
			string filename;
			line_stream >> filename;
			//�}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}


	}


}

void Model::LoadMaterial(const std::string &directoryPath, const std::string &filename)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	//�}�e���肠��t�@�C�����J��
	file.open(directoryPath + filename);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}

	//1�s���ǂݍ���
	string line;
	while (getline(file, line))
	{
		//1�s���̕�������X�g���[���ɕϊ�
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̂�擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�擪�̃^�u�����͖�������
		if (key[0] == '\t')
		{
			key.erase(key.begin());//�擪�̕������폜
		}

		if (key == "newmtl")
		{
			//�}�e���A�����ǂݍ���
			line_stream >> model.material.name;
		}

		//�擪������Ka�Ȃ�A���r�G���F
		if (key == "Ka")
		{
			line_stream >> model.material.ambient.x;
			line_stream >> model.material.ambient.y;
			line_stream >> model.material.ambient.z;
		}

		//�擪������Kd�Ȃ�f�B�t�[�Y�F
		if (key == "Kd")
		{
			line_stream >> model.material.diffuse.x;
			line_stream >> model.material.diffuse.y;
			line_stream >> model.material.diffuse.z;
		}

		//�擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks")
		{
			line_stream >> model.material.specular.x;
			line_stream >> model.material.specular.y;
			line_stream >> model.material.specular.z;
		}

		//�擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd")
		{
			//�e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> model.material.textureFilename;
			//�e�N�X�`���ǂݍ���
			LoadTexture(directoryPath, model.material.textureFilename);
		}
	}

}

void Model::LoadTexture(const std::string &directoryPath, const std::string &filename)
{
	//�t�@�C���p�X������
	string filepath = directoryPath + filename;

	//���j�R�[�h������ɕϊ�����
	wchar_t wfilepath[128];
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0,
		filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	model.textureHandle = TextureMgr::Instance()->SpriteLoadTexture(wfilepath);
}

ModelObject Model::GetModel()
{
	return model;
}

void Model::SetModel(ModelObject model)
{
	this->model = model;
}
