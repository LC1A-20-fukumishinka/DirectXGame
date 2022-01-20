#include "Sprite.h"
#include "TextureMgr.h"
#include "MyDirectX.h"
Sprite::Sprite()
{
	anchorpoint = { 0.5f, 0.5f };
	isFlipX = false;
	isFlipY = false;
	position = { 0, 0, 0 };
	color = { 1, 1, 1, 1 };
	rotation = 0.0f;

	texNumber = 0;
	size = { 100, 100 };
	texLeftTop = { 0, 0 };
	texSize = { 0 , 0 };
	isInvisible = false;
}

void Sprite::Init(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	HRESULT result = S_FALSE;
	MyDirectX* myD = MyDirectX::GetInstance();

	SpriteCommon::Instance();
	this->texNumber = texNumber;

	if (!TextureMgr::Instance()->CheckHandle(texNumber))
	{
		assert(0);
		return;
	}

	enum { LB, LT, RB, RT };
	this->anchorpoint = anchorpoint;
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
	position = { 0, 0, 0 };
	color = { 1, 1, 1, 1 };
	rotation = 0.0f;

	////頂点データ
	VertexPosUv vertices[] =
	{
		{},//左下
		{},//左上
		{},//右下
		{},//右上
	};


	////切り取りサイズを画像のサイズに合わせて変更
	D3D12_RESOURCE_DESC resDesc = TextureMgr::Instance()->GetTexBuff(texNumber)->GetDesc();
	texLeftTop = { 0, 0 };
	texSize = { (float)resDesc.Width , (float)resDesc.Height };


	//頂点バッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	SpriteTransferVertexBuffer();

	//頂点バッファビューの作成
	vBView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vBView.SizeInBytes = sizeof(vertices);
	vBView.StrideInBytes = sizeof(vertices[0]);

	//定数バッファの生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = DirectX::XMFLOAT4(1, 1, 1, 1);//色指定(RGBA)
	constMap->mat = SpriteCommon::Instance()->matProjection;	//平行透視投影
	constBuff->Unmap(0, nullptr);


}

//頂点バッファ系の設定
void Sprite::SpriteTransferVertexBuffer()
{

	if (!TextureMgr::Instance()->CheckHandle(texNumber))
	{
		assert(0);
		return;
	}
	HRESULT result = S_FALSE;
	enum { LB, LT, RB, RT };

	//頂点データ
	VertexPosUv vertices[] = {
		{{}, {0.0f, 1.0f}},//左下
		{{}, {0.0f, 0.0f}},//左上
		{{}, {1.0f, 1.0f}},//右下
		{{}, {1.0f, 0.0f}},//右上
	};

	float left = (0.0f - anchorpoint.x);
	float right = (1.0f - anchorpoint.x);
	float top = (0.0f - anchorpoint.y);
	float bottom = (1.0f - anchorpoint.y);

	if (isFlipX)
	{
		left = -left;
		right = -right;
	}

	if (isFlipY)
	{
		top = -top;
		bottom = -bottom;
	}

	//頂点データ
	vertices[LB].pos = { left,bottom,  0.0f };//左下
	vertices[LT].pos = { left, top,  0.0f };//左上
	vertices[RB].pos = { right,bottom,  0.0f };//右下
	vertices[RT].pos = { right, top,  0.0f };//右上


		//テクスチャ情報取得
	D3D12_RESOURCE_DESC resDesc = TextureMgr::Instance()->GetTexBuff(texNumber)->GetDesc();


	float tex_left = texLeftTop.x / resDesc.Width;
	float tex_right = (texLeftTop.x + texSize.x) / resDesc.Width;
	float tex_top = texLeftTop.y / resDesc.Height;
	float tex_bottom = (texLeftTop.y + texSize.y) / resDesc.Height;

	vertices[LB].uv = { tex_left, tex_bottom };
	vertices[LT].uv = { tex_left, tex_top };
	vertices[RB].uv = { tex_right, tex_bottom };
	vertices[RT].uv = { tex_right, tex_top };


	//バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);
}


void Sprite::SpriteDraw()
{

	//描画フラグがtrueじゃなかったら早期リターン
	if (isInvisible) return;

	if (!TextureMgr::Instance()->CheckHandle(texNumber))
	{
		assert(0);
		return;
	}

	MyDirectX* myD = MyDirectX::GetInstance();
	//パイプランステートの設定
	myD->GetCommandList()->SetPipelineState(SpriteCommon::Instance()->pipelineSet.pipelineState.Get());
	//ルートシグネチャの設定
	myD->GetCommandList()->SetGraphicsRootSignature(SpriteCommon::Instance()->pipelineSet.rootSignature.Get());
	//プリミティブ形状を設定
	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { TextureMgr::Instance()->GetDescriptorHeap() };
	myD->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);


	//頂点バッファをセット
	myD->GetCommandList()->IASetVertexBuffers(0, 1, &vBView);

	//定数バッファをセット
	myD->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//シェーダーリソースビューをセット
	myD->GetCommandList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			TextureMgr::Instance()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	//描画コマンド
	myD->GetCommandList()->DrawInstanced(4, 1, 0, 0);

}

//定数バッファ系の設定(色とアフィン変換)
void Sprite::SpriteUpdate()
{
	//ワールド行列の更新
	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= DirectX::XMMatrixScaling(size.x, size.y, 1.0f);

	//Z軸回転
	matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation));
	//平行移動
	matWorld *= DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//定数バッファの転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matWorld * SpriteCommon::Instance()->matProjection;
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

}
