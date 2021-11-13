#pragma once
#include "../Base/MyDirectX.h"
#include <DirectXMath.h>
#include "SpriteCommon.h"


class Sprite
{
public:
	Sprite();

	void Init(int window_width, int window_height, UINT texnumber, const SpriteCommon &spriteCommon, DirectX::XMFLOAT2 anchorpoint = { 0.5f, 0.5f }, bool isFlipX = false, bool isFlipY = false);

	void SpriteTransferVertexBuffer(const SpriteCommon &spriteCommon);

	//スプライトの描画コマンド
	void SpriteDraw(const SpriteCommon &spriteCommon);
	//スプライト情報の更新(行列計算や、色変更など)
	void SpriteUpdate(const SpriteCommon &spriteCommon);


	//座標
	DirectX::XMFLOAT3 position;
	//Z軸回りの回転軸
	float rotation;
	//色(RGBA)
	DirectX::XMFLOAT4 color;
	//ワールド行列
	DirectX::XMMATRIX matWorld;
	//テクスチャ番号
	UINT texNumber;
	//大きさ
	DirectX::XMFLOAT2 size;
	//アンカーポイント
	DirectX::XMFLOAT2 anchorpoint;
	//左右反転
	bool isFlipX;
	//上下反転
	bool isFlipY;
	//テクスチャの左上座標
	DirectX::XMFLOAT2 texLeftTop;
	//テクスチャ切り出しサイズ
	DirectX::XMFLOAT2 texSize;
	//非表示
	bool isInvisible;
private:
	struct ConstBufferData
	{
		DirectX::XMFLOAT4 color;	//色(RGBA)
		DirectX::XMMATRIX mat;	//3D変換行列
	};
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos;	//xyz座標
		DirectX::XMFLOAT2 uv;	//uv座標
	};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;				//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;				//定数バッファ
	D3D12_VERTEX_BUFFER_VIEW vBView{};			//頂点バッファビュー

};

//スプライトコモンのコマンド(spriteDrawの前に必要)
void SpriteCommonBeginDraw(const SpriteCommon &spriteCommon);
