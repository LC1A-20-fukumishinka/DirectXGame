#include "Object3DCommon.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>


using namespace DirectX;
Object3DCommon::Object3DCommon()
{
	pipelineSet = {};
	matProjection = DirectX::XMMatrixIdentity();
}

void Object3DCommon::Init()
{
	MyDirectX *myDirectX = MyDirectX::GetInstance();

	//視錐台行列の生成
	matProjection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),				//上下画角60度
		(float)myDirectX->winWidth / (float)myDirectX->winHeight,	//アスペクト比(画面横幅/画面縦幅)
		0.1f, 1000.0f							//前端、奥端
	);

	//ビュー行列
	float angle = 0.0f;	//カメラの回転角
	//DirectX::XMFLOAT3 eye(0, 0, -100);
	//DirectX::XMFLOAT3 target(0, 0, 0);
	//DirectX::XMFLOAT3 up(0, 1, 0);
	//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	//パイプラインの生成
	Object3DCreateGraphPipeline(myDirectX->GetDevice());

	//デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = Object3DSRVCount;
	HRESULT result = myDirectX->GetDevice()->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&descHeap));

	CornInit();

	BoxInit();

	PlaneInit();
}

void Object3DCommon::SpriteLoadTexture(UINT texnumber, const wchar_t *filename)
{
	MyDirectX *myDirectX = MyDirectX::GetInstance();
	//異常な番号の指定を検出
	assert(texnumber <= Object3DSRVCount - 1);
	HRESULT result;

	//WICテクスチャのロード
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,	//「Resources」フォルダの「texuture.png」
		DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const DirectX::Image *img = scratchImg.GetImage(0, 0, 0);	//生データ抽出

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);
#pragma region texBuff

	result = myDirectX->GetDevice()->CreateCommittedResource(	//GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&texBuff[texnumber]));
#pragma endregion

#pragma region texDataSend
	result = texBuff[texnumber]->WriteToSubresource(
		0,
		nullptr,							//全領域へコピー
		img->pixels,							//元データアドレス
		(UINT16)img->rowPitch,		//1ラインサイズ
		(UINT16)img->slicePitch	//全サイズ
	);

	//delete[] imageData;
#pragma endregion
#pragma region SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};		//設定構造体
	srvDesc.Format = metadata.format;	//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	myDirectX->GetDevice()->CreateShaderResourceView(
		texBuff[texnumber].Get(),	//ビューと関連付けるバッファ
		&srvDesc,							//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(), texnumber,
			myDirectX->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
#pragma endregion


}

void Object3DCommon::Object3DCreateGraphPipeline(ID3D12Device *dev)
{

	HRESULT result;
	Microsoft::WRL::ComPtr<ID3DBlob>vsBlob;//頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob>psBlob;//ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob;//エラーオブジェクト
	//頂点シェーダの読み込みとコンパイル
#pragma region VShader
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用指定
		0,
		&vsBlob, &errorBlob);
#pragma endregion
	//ピクセルシェーダー読み込み
#pragma region PShader
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result))
	{
		//errorBlobからエラーの内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

#pragma endregion
	//頂点レイアウトの宣言と設定
#pragma region inputLayOut
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",									//セマンティック
			0,											//同じセマンティック名が複数あるときに使うインデックス(0で良い)
			DXGI_FORMAT_R32G32B32_FLOAT,				//要素数とビット数を表す(XYZの3つでfloat型なので"R32G32B32_FLOAT")
			0,											//入力スロットインデックス(0でよい)
			D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
			0											//一度に描画するインスタンス数(0でよい)
		},//(1行で書いた方が見やすいらしい)
		//法線ベクトル
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//座標以外に、色、テクスチャなどを渡す場合はさらに続ける
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};
#pragma endregion
	//パイプラインステート設定変数の宣言と、各種項目の設定
#pragma region pipelineState
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//標準的な設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA全てのチャンネルを描画

	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];

	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソース(描画する図形のピクセル)の値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デスト(描画対象ピクセル　　　)の値を  0%使う

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f - ソースのアルファ値


	//デプスステンシルステートの設定
#pragma region DepthStencilState

	//標準定期な設定(深度テストを行う、書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;								//深度値フォーマット
#pragma endregion

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;//描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0^255指定のRGBA
	gpipeline.SampleDesc.Count = 1;//ピクセルにつき1回サンプリング
#pragma endregion
	//ルートシグネチャの生成
#pragma region RootSignature
#pragma region rootParameter
//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;

	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	//t0 レジスタ
	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
#pragma endregion

#pragma region textureSampler
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

#pragma endregion
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob>rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result =
		D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result =
		dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
			rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootSignature));
	gpipeline.pRootSignature = pipelineSet.rootSignature.Get();

	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelineState));

#pragma endregion

}

void Object3DCommon::Object3DCommonBeginDraw()
{
	MyDirectX *myD = MyDirectX::GetInstance();
	//パイプランステートの設定
	myD->GetCommandList()->SetPipelineState(pipelineSet.pipelineState.Get());
	//ルートシグネチャの設定
	myD->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	//プリミティブ形状を設定
	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Object3DCommon::CornInit()
{

	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();

	Vertex vertices[5] = {};

	unsigned short indices[CornNumIndices] = {};
	//頂点バッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&cornVertBuff)
	);

	//インデックスバッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//インデックス情報が入る分のサイズ
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&cornIndexBuff));


	//頂点バッファとインデックスバッファの設定
	CornTransferIndexBufferVertexBuffer();

	int size = sizeof(indices);
	//頂点バッファビューの作成
	cornVBView.BufferLocation = cornVertBuff->GetGPUVirtualAddress();
	cornVBView.SizeInBytes = sizeof(vertices);
	cornVBView.StrideInBytes = sizeof(vertices[0]);

	cornIBView.BufferLocation = cornIndexBuff->GetGPUVirtualAddress();
	cornIBView.Format = DXGI_FORMAT_R16_UINT;
	cornIBView.SizeInBytes = sizeof(indices);
}

void Object3DCommon::CornTransferIndexBufferVertexBuffer()
{
	const float topHeight = 1.0f;
	float radius = 1.0f;
	float pi = XM_PI;
	Vertex vertices[5];
#pragma region pos
	for (int i = 0; i < 3; i++)
	{
		vertices[i].pos =
		{
			radius * sinf(2 * pi / 3 * i),
			radius * cosf(2 * pi / 3 * i),
			0
		};
	}
	vertices[3].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertices[4].pos = XMFLOAT3(0.0f, 0.0f, -topHeight);

#pragma endregion

#pragma region uv
	for (int i = 0; i < 5; i++)
	{
		vertices[i].uv = XMFLOAT2(0, 0);
	}

	vertices[4].uv = XMFLOAT2(1, 1);
#pragma endregion

#pragma region IndexData
	unsigned short indices[CornNumIndices] = {};
	for (int i = 0; i < 3; i++)
	{
		indices[i * 3 + 1] = i;
		indices[i * 3 + 9] = i;
		if (i >= 2)
		{
			indices[i * 3] = 0;
			indices[i * 3 + 1 + 9] = 0;
		}
		else
		{
			indices[i * 3] = i + 1;
			indices[i * 3 + 1 + 9] = i + 1;
		}

		indices[i * 3 + 2] = 3;
		indices[i * 3 + 2 + 9] = 4;
	}

#pragma endregion

#pragma region Normal
	for (int i = 0; i < _countof(indices) / 3; i++)
	{

		//三角形1つ毎に計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short i0 = indices[i * 3 + 0];
		unsigned short i1 = indices[i * 3 + 1];
		unsigned short i2 = indices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[i0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[i1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[i2].pos);

		//p0->p1ベクトル、p0->p2ベクトルを計算(ベクトルの計算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//正規化(長さを1にする)
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[i0].normal, normal);
		XMStoreFloat3(&vertices[i1].normal, normal);
		XMStoreFloat3(&vertices[i2].normal, normal);
	}
#pragma endregion

	//GPU上のバッファに対応した仮想メモリを取得
	Vertex *vertMap = nullptr;
	HRESULT result = cornVertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	//マップを解除
	cornVertBuff->Unmap(0, nullptr);

	//GPU乗のバッファに「対応した仮想メモリの取得
	unsigned short *indexMap = nullptr;
	result = cornIndexBuff->Map(0, nullptr, (void **)&indexMap);

	memcpy(indexMap, indices, sizeof(indices));

	//繋がりを解除
	cornIndexBuff->Unmap(0, nullptr);

}

void Object3DCommon::BoxInit()
{

	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();

	Vertex vertices[24] = {};

	unsigned short indices[BoxNumIndices] = {};	
	//頂点バッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&boxVertBuff)
	);

	//インデックスバッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//インデックス情報が入る分のサイズ
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&boxIndexBuff));


	//頂点バッファとインデックスバッファの設定
	BoxTransferIndexBufferVertexBuffer();

	//頂点バッファビューの作成
	boxVBView.BufferLocation = boxVertBuff->GetGPUVirtualAddress();
	boxVBView.SizeInBytes = sizeof(vertices);
	boxVBView.StrideInBytes = sizeof(vertices[0]);

	boxIBView.BufferLocation = boxIndexBuff->GetGPUVirtualAddress();
	boxIBView.Format = DXGI_FORMAT_R16_UINT;
	boxIBView.SizeInBytes = sizeof(indices);
}


void Object3DCommon::BoxTransferIndexBufferVertexBuffer()
{
#pragma region VertexData
	Vertex vertices[] = {
	//前
	{{ -1.0f, -1.0f,  -1.0f},{},{0.0f, 1.0f}},	//左下
	{{ -1.0f,  1.0f,  -1.0f},{},{0.0f, 0.0f}},	//左上
	{{  1.0f, -1.0f,  -1.0f},{},{1.0f, 1.0f}},	//右下
	{{  1.0f,  1.0f,  -1.0f},{},{1.0f, 0.0f}},	//右上

	//後
	{{  1.0f, -1.0f,   1.0f},{},{1.0f, 1.0f}},	//右下
	{{  1.0f,  1.0f,   1.0f},{},{1.0f, 0.0f}},	//右上
	{{ -1.0f, -1.0f,   1.0f},{},{0.0f, 1.0f}},	//左下
	{{ -1.0f,  1.0f,   1.0f},{},{0.0f, 0.0f}},	//左上


	//左
	{{  -1.0f, -1.0f, -1.0f},{},{0.0f, 1.0f}},	//左下
	{{  -1.0f, -1.0f,  1.0f},{},{0.0f, 0.0f}},	//左上
	{{  -1.0f,  1.0f, -1.0f},{},{1.0f, 1.0f}},	//右下
	{{  -1.0f,  1.0f,  1.0f},{},{1.0f, 0.0f}},	//右上

	//右
	{{   1.0f,  1.0f, -1.0f},{},{1.0f, 1.0f}},	//右下
	{{   1.0f,  1.0f,  1.0f},{},{1.0f, 0.0f}},	//右上
	{{   1.0f, -1.0f, -1.0f},{},{0.0f, 1.0f}},	//左下
	{{   1.0f, -1.0f,  1.0f},{},{0.0f, 0.0f}},	//左上


	//下
	{{ -1.0f,  -1.0f, -1.0f},{},{0.0f, 1.0f}},	//左下
	{{  1.0f,  -1.0f, -1.0f},{},{0.0f, 0.0f}},	//左上
	{{ -1.0f,  -1.0f,  1.0f},{},{1.0f, 1.0f}},	//右下
	{{  1.0f,  -1.0f,  1.0f},{},{1.0f, 0.0f}},	//右上

	//上
	{{ -1.0f,  1.0f,  1.0f},{},{1.0f, 1.0f}},	//右下
	{{  1.0f,  1.0f,  1.0f},{},{1.0f, 0.0f}},	//右上
	{{ -1.0f,  1.0f, -1.0f},{},{0.0f, 1.0f}},	//左下
	{{  1.0f,  1.0f, -1.0f},{},{0.0f, 0.0f}},	//左上

};
#pragma endregion


#pragma region IndexData
	unsigned short indices[BoxNumIndices] = {};

	//インデックスに値を入れる
	for (int i = 0; i < 6; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}
#pragma endregion

#pragma region Normal

	//インデックスを使って法線をも尾メル
	for (int i = 0; i < _countof(indices) / 3; i++)
	{

		//三角形1つ毎に計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short i0 = indices[i * 3 + 0];
		unsigned short i1 = indices[i * 3 + 1];
		unsigned short i2 = indices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[i0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[i1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[i2].pos);

		//p0->p1ベクトル、p0->p2ベクトルを計算(ベクトルの計算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//正規化(長さを1にする)
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[i0].normal, normal);
		XMStoreFloat3(&vertices[i1].normal, normal);
		XMStoreFloat3(&vertices[i2].normal, normal);
	}


#pragma endregion
	//GPU上のバッファに対応した仮想メモリを取得
	Vertex *vertMap = nullptr;
	HRESULT result = boxVertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));

	//マップを解除
	boxVertBuff->Unmap(0, nullptr);

	//GPU上のバッファに対応した仮想メモリの取得
	unsigned short *indexMap = nullptr;
	result = boxIndexBuff->Map(0, nullptr, (void **)&indexMap);

	memcpy(indexMap, indices, sizeof(indices));
	//繋がりを解除
	boxIndexBuff->Unmap(0, nullptr);

}

void Object3DCommon::PlaneInit()
{
	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();

	Vertex vertices[4] = {};

	unsigned short indices[PlaneNumIndices] = {};
	//頂点バッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&planeVertBuff)
	);

	//インデックスバッファ生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//インデックス情報が入る分のサイズ
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&planeIndexBuff));


	//頂点バッファとインデックスバッファの設定
	PlaneTransferIndexBufferVertexBuffer();

	//頂点バッファビューの作成
	planeVBView.BufferLocation = planeVertBuff->GetGPUVirtualAddress();
	planeVBView.SizeInBytes = sizeof(vertices);
	planeVBView.StrideInBytes = sizeof(vertices[0]);

	planeIBView.BufferLocation = planeIndexBuff->GetGPUVirtualAddress();
	planeIBView.Format = DXGI_FORMAT_R16_UINT;
	planeIBView.SizeInBytes = sizeof(indices);
}

void Object3DCommon::PlaneTransferIndexBufferVertexBuffer()
{
#pragma region VertexData
	Vertex vertices[] = {
		//前
		{{ -1.0f, -1.0f,  0.0f},{},{0.0f, 1.0f}},	//左下
		{{ -1.0f,  1.0f,  0.0f},{},{0.0f, 0.0f}},	//左上
		{{  1.0f, -1.0f,  0.0f},{},{1.0f, 1.0f}},	//右下
		{{  1.0f,  1.0f,  0.0f},{},{1.0f, 0.0f}},	//右上
	};
#pragma endregion


#pragma region IndexData
	unsigned short indices[PlaneNumIndices] = 
	{
		0,1,2,
		2,1,3
	};

#pragma endregion

#pragma region Normal

	//インデックスを使って法線をも尾メル
	for (int i = 0; i < _countof(indices) / 3; i++)
	{

		//三角形1つ毎に計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short i0 = indices[i * 3 + 0];
		unsigned short i1 = indices[i * 3 + 1];
		unsigned short i2 = indices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[i0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[i1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[i2].pos);

		//p0->p1ベクトル、p0->p2ベクトルを計算(ベクトルの計算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//正規化(長さを1にする)
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[i0].normal, normal);
		XMStoreFloat3(&vertices[i1].normal, normal);
		XMStoreFloat3(&vertices[i2].normal, normal);
	}


#pragma endregion
	//GPU上のバッファに対応した仮想メモリを取得
	Vertex *vertMap = nullptr;
	HRESULT result = planeVertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));

	//マップを解除
	planeVertBuff->Unmap(0, nullptr);

	//GPU上のバッファに対応した仮想メモリの取得
	unsigned short *indexMap = nullptr;
	result = planeIndexBuff->Map(0, nullptr, (void **)&indexMap);

	memcpy(indexMap, indices, sizeof(indices));
	//繋がりを解除
	planeIndexBuff->Unmap(0, nullptr);
}
