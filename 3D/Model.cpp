#include "Model.h"
#include <d3dx12.h>
#include "MyDirectX.h"
#include <fstream>//ファイル入出力
#include <sstream>//文字列に対する入出力
#include <string>//文字列
#include <vector>//頂点データを纏める
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
	const string directoryPath = "Resources/" + modelname + "/";	//”Resouces/triangle_mat/”

	LoadModel(directoryPath, modelname);

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * model.vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * model.indices.size());

	ID3D12Device *device = MyDirectX::GetInstance()->GetDevice();
	// 頂点バッファ生成
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

	// 頂点バッファへのデータ転送
	VertexPosNormalUv *vertMap = nullptr;
	result = model.vertBuff->Map(0, nullptr, (void **)&vertMap);
	if (SUCCEEDED(result)) {
		//memcpy(vertMap, vertices, sizeof(vertices));
		std::copy(model.vertices.begin(), model.vertices.end(), vertMap);
		model.vertBuff->Unmap(0, nullptr);
	}


	// インデックスバッファ生成
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

	// インデックスバッファへのデータ転送
	unsigned short *indexMap = nullptr;
	result = model.indexBuff->Map(0, nullptr, (void **)&indexMap);
	if (SUCCEEDED(result)) {

		// 全インデックスに対して
		//for (int i = 0; i < _countof(indices); i++)
		//{
		//	indexMap[i] = indices[i];	// インデックスをコピー
		//}
		std::copy(model.indices.begin(), model.indices.end(), indexMap);
		model.indexBuff->Unmap(0, nullptr);
	}



	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model.constBuffB1));

	//定数バッファへデータ転送
	ConstBufferDataB1 *constMap = nullptr;
	result = model.constBuffB1->Map(0, nullptr, (void **)&constMap);
	constMap->ambient = model.material.ambient;
	constMap->diffuse = model.material.diffuse;
	constMap->specular = model.material.specular;
	constMap->alpha = model.material.alpha;
	model.constBuffB1->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	model.vbView.BufferLocation = model.vertBuff->GetGPUVirtualAddress();
	//vbView.SizeInBytes = sizeof(vertices);
	model.vbView.SizeInBytes = sizeVB;
	model.vbView.StrideInBytes = sizeof(model.vertices[0]);

	// インデックスバッファビューの作成
	model.ibView.BufferLocation = model.indexBuff->GetGPUVirtualAddress();
	model.ibView.Format = DXGI_FORMAT_R16_UINT;
	//ibView.SizeInBytes = sizeof(indices);
	model.ibView.SizeInBytes = sizeIB;
}


void Model::LoadModel(const std::string &directoryPath, const std::string &modelname)
{

	//ファイルストリーム
	std::ifstream file;
	////.objファイルを開く
	//file.open("Resources/triangle_tex/triangle_tex.obj");
	const string filename = modelname + ".obj";	//”triangle_mat.obj”
	file.open(directoryPath + filename);	//”Resouces/triangle_mat/triangle_mat.obj”

	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	vector<XMFLOAT3> positions;		//頂点情報
	vector<XMFLOAT3> normals;		//法線ベクトル
	vector<XMFLOAT2> texcoords;		//テクスチャUV
	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする4
		std::istringstream line_stream(line);

		//半角スペース区切り出行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭文字列がvなら頂点座標
		if (key == "v")
		{
			//X,Y,Z座標読み込み
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//座標データに追加
			positions.emplace_back(position);
			////頂点データに追加
			//VertexPosNormalUv vertex{};
			//vertex.pos = position;
			//vertices.emplace_back(vertex);
		}

		//先頭文字列がvtならテクスチャ
		if (key == "vt")
		{
			//U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}

		//先頭文字列がvnなら法線ベクトル
		if (key == "vn")
		{
			//X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}

		//先頭文字列がfならポリゴン (三角形)
		if (key == "f")
		{
			//半角スペース区切り出行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//頂点インデックス1っ個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
				index_stream >> indexNormal;

				VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				model.vertices.emplace_back(vertex);
				//頂点インデックスに追加
				model.indices.emplace_back((unsigned short)model.indices.size());

				//const int squareSurfaceVerticesCount = 4;
				////面の頂点数が4
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
			//マテリアルのファイルの読み込み
			string filename;
			line_stream >> filename;
			//マテリアル読み込み
			LoadMaterial(directoryPath, filename);
		}


	}


}

void Model::LoadMaterial(const std::string &directoryPath, const std::string &filename)
{
	//ファイルストリーム
	std::ifstream file;
	//マテありあるファイルを開く
	file.open(directoryPath + filename);
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//1行分の文字列をストリームに変換
		std::istringstream line_stream(line);

		//半角スペース区切りで行のれ先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視する
		if (key[0] == '\t')
		{
			key.erase(key.begin());//先頭の文字を削除
		}

		if (key == "newmtl")
		{
			//マテリアル冥読み込み
			line_stream >> model.material.name;
		}

		//先頭文字列がKaならアンビエン色
		if (key == "Ka")
		{
			line_stream >> model.material.ambient.x;
			line_stream >> model.material.ambient.y;
			line_stream >> model.material.ambient.z;
		}

		//先頭文字列がKdならディフーズ色
		if (key == "Kd")
		{
			line_stream >> model.material.diffuse.x;
			line_stream >> model.material.diffuse.y;
			line_stream >> model.material.diffuse.z;
		}

		//先頭文字列がKsならスペキュラー色
		if (key == "Ks")
		{
			line_stream >> model.material.specular.x;
			line_stream >> model.material.specular.y;
			line_stream >> model.material.specular.z;
		}

		//先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd")
		{
			//テクスチャのファイル名読み込み
			line_stream >> model.material.textureFilename;
			//テクスチャ読み込み
			LoadTexture(directoryPath, model.material.textureFilename);
		}
	}

}

void Model::LoadTexture(const std::string &directoryPath, const std::string &filename)
{
	//ファイルパスを結合
	string filepath = directoryPath + filename;

	//ユニコード文字列に変換する
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
