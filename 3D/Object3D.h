#pragma once
#include "../Base/MyDirectX.h"
#include <DirectXMath.h>
#include "Object3DCommon.h"
#include "../Camera.h"
#include "GraphicsPipeline.h"
class Object3D
{
public:

	struct ConstBufferData
	{
		DirectX::XMFLOAT4 color;	//色(RGBA)
		DirectX::XMMATRIX mat;	//3D変換行列
	};
	enum ObjectName
	{
		Corn,
		Box,
		Plane
	};
	Object3D();

	void Init(const Object3DCommon &object3DCommon,const Camera &camera, UINT texNumber, Object3D *parent = nullptr);

	//void Object3DTranceferVertexBuffer(const Object3DCommon &objCommon);

	//void Objecte3DDraw();

	//void Object3DUpdate();

	//void DrawObject3D(ID3D12DescriptorHeap *descHeap, D3D12_VERTEX_BUFFER_VIEW &vbView, D3D12_INDEX_BUFFER_VIEW &iBView, UINT numIndices);

	DirectX::XMMATRIX GetMatWorld();

	void Update(const Camera &camera);

	void SetConstBuffer(const Camera &camera);

	void Draw(const Object3DCommon &object3DCommon, PipeClass::PipelineSet pipelineSet, int textureNumber);

	void SetParent(Object3D *parent);
	//色(RGBA)
	DirectX::XMFLOAT4 color;
	//大きさ
	DirectX::XMFLOAT3 scale;
	//回転
	DirectX::XMFLOAT3 rotation;
	//座標
	DirectX::XMFLOAT3 position;
	//描画フラグ
	bool isInvisible;
	//どんな形か
	ObjectName type;

	//ワールド行列
	DirectX::XMMATRIX matWorld;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

private:

	//テクスチャ番号
	UINT texNumber;

	Object3D *parent;
	//非表示




	//要らない

	//struct Vertex
	//{
	//	DirectX::XMFLOAT3 pos;	//xyz座標
	//	DirectX::XMFLOAT3 normal;
	//	DirectX::XMFLOAT2 uv;	//uv座標
	//};
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;				//頂点バッファ
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
	//D3D12_VERTEX_BUFFER_VIEW vBView{};			//頂点バッファビュー
	//D3D12_INDEX_BUFFER_VIEW iBView{};

	//void CornInit(const Object3DCommon &object3DCommon);
	//void BoxInit(const Object3DCommon &object3DCommon);



	//void CornTransferIndexBufferVertexBuffer(const Object3DCommon &object3DCommon);
};

//オブジェクト3Dのコマンド(Object3DDrawの前に必要)
//void Object3DCommonBeginDraw(const Object3DCommon &Object3DCommon, const SpriteCommon &spriteCommon);
