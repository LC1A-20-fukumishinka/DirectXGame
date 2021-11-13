#pragma once
#include "../Base/MyDirectX.h"
#include<DirectXMath.h>



class SpriteCommon
{
public:
	enum Sprites
	{
		spriteSRVCount = 512
	};
	//パイプラインセット
	PipelineSet pipelineSet;
	//射影行列
	DirectX::XMMATRIX matProjection{};

	//テクスチャ用デスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descHeap;
	//テクスチャリソース(テクスチャバッファ)の配列
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[spriteSRVCount];


	void Init();

	void SpriteLoadTexture(UINT texnumber, const wchar_t *filename);
private:

	MyDirectX *myDirectX;
	//スプライトのパイプライン設定関数
void SpriteCreateGraphPipeline(ID3D12Device *dev);
};