#pragma once
#include "../Base/MyDirectX.h"
#include<DirectXMath.h>


class Object3DCommon
{
public:
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
	//パイプライン設定用構造体
	PipelineSet pipelineSet;
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
	void Object3DCommonBeginDraw();
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



	void Object3DCreateGraphPipeline(ID3D12Device *dev);

};