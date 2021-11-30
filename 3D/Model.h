#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <string>
struct VertexPosNormalUv
{
	DirectX::XMFLOAT3 pos; // xyz座標
	DirectX::XMFLOAT3 normal; // 法線ベクトル
	DirectX::XMFLOAT2 uv;  // uv座標
};

struct ConstBufferDataB1
{
	DirectX::XMFLOAT3 ambient;	//アンビエント係数
	float pad1;			//パディング
	DirectX::XMFLOAT3 diffuse;	//ディフーズ係数
	float pad2;			//パディング
	DirectX::XMFLOAT3 specular;	//スペキュラー係数
	float alpha;		//アルファ
};

struct ModelObject
{
	//マテリアル
	struct Material
	{
		std::string name;
		DirectX::XMFLOAT3 ambient;
		DirectX::XMFLOAT3 diffuse;
		DirectX::XMFLOAT3 specular;
		float alpha;
		std::string textureFilename;

		//コンストラクタ
		Material()
		{
			ambient = { 0.3f, 0.3f, 0.3f };
			diffuse = { 0.0f, 0.0f, 0.0f };
			specular = { 0.0f, 0.0f, 0.0f };
			alpha = 1.0f;
		}
	};

	std::vector<VertexPosNormalUv> vertices;
	std::vector<unsigned short> indices;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
	//マテリアル用の定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB1;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;

	//マテリアル
	Material material;


	//テクスチャー用のハンドル
	int textureHandle;
};
class Model
{
public:
	Model();
	~Model();

	void CreateModel( const std::string &modelname);
	void LoadModel(const std::string &directoryPath, const std::string &modelname);
	void LoadMaterial(const std::string &directoryPath, const std::string &filename);
	void LoadTexture(const std::string &directoryPath, const std::string &filename);

	ModelObject GetModel();
	void SetModel(ModelObject model);
private:
	ModelObject model;
};