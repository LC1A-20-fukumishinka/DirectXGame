#pragma once
#include "../Base/MyDirectX.h"
#include<DirectXMath.h>

class Object3DCommon
{
public:
	struct PipelineSet
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	//ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;	//パイプラインステート
	};
	enum Sprites
	{
		Object3DSRVCount = 512
	};
	enum
	{
		CornNumIndices = 18,
		BoxNumIndices = 36,
		PlaneNumIndices = 6
	};
	Object3DCommon();
	//射影行列
	DirectX::XMMATRIX matProjection{};

	//DirectX::XMMATRIX matView{};

	//テクスチャ用デスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descHeap;
	//テクスチャリソース(テクスチャバッファ)の配列
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[Object3DSRVCount];
	void Init();

	void SpriteLoadTexture(UINT texnumber, const wchar_t *filename);

	Microsoft::WRL::ComPtr<ID3D12Resource> cornVertBuff;				//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> cornIndexBuff;
	D3D12_VERTEX_BUFFER_VIEW cornVBView{};			//頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW cornIBView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> boxVertBuff;				//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> boxIndexBuff;
	D3D12_VERTEX_BUFFER_VIEW boxVBView{};			//頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW boxIBView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> planeVertBuff;				//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> planeIndexBuff;
	D3D12_VERTEX_BUFFER_VIEW planeVBView{};			//頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW planeIBView{};
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;	//xyz座標
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;	//uv座標
	};



	void CornInit();
	void BoxInit();
	void PlaneInit();


	void CornTransferIndexBufferVertexBuffer();
	void BoxTransferIndexBufferVertexBuffer();
	void PlaneTransferIndexBufferVertexBuffer();
};