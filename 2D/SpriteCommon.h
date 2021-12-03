#pragma once
#include "../Base/MyDirectX.h"
#include<DirectXMath.h>



class SpriteCommon
{
public:
	struct PipelineSet
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};
	enum Sprites
	{
		spriteSRVCount = 512
	};

	PipelineSet pipelineSet;
	//射影行列
	DirectX::XMMATRIX matProjection{};

	//デスクリプタヒープ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descHeap;
	//テクスチャーバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff[spriteSRVCount];

	void Init();
	void SpriteLoadTexture(UINT texnumber, const wchar_t *filename);
private:

	MyDirectX *myDirectX;

	void SpriteCreateGraphPipeline(ID3D12Device *dev);
};