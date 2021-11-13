#include "SpriteCommon.h"
#include <d3dcompiler.h>
#include "../Base/MyDirectX.h"
#include <DirectXTex.h>
void SpriteCommon::Init()
{
	myDirectX = MyDirectX::GetInstance();
	HRESULT result = S_FALSE;

	//スプライト用パイプライン生成
	SpriteCreateGraphPipeline(myDirectX->GetDevice());


	//平行投影の射影行列生成
	matProjection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, (float)myDirectX->winWidth, (float)myDirectX->winHeight, 0.0f, 0.0f, 1.0f);

	//デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = myDirectX->GetDevice()->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&descHeap));
}

void SpriteCommon::SpriteLoadTexture(UINT texnumber, const wchar_t *filename)
{
	//異常な番号の指定を検出
	assert(texnumber <= spriteSRVCount - 1);
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

//パイプラインとルートシグネチャのセット
void SpriteCommon::SpriteCreateGraphPipeline(ID3D12Device *dev)
{
	HRESULT result;
	Microsoft::WRL::ComPtr<ID3DBlob>vsBlob;//頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob>psBlob;//ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob;//エラーオブジェクト
	//頂点シェーダの読み込みとコンパイル
#pragma region VShader
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",//シェーダファイル名
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
		L"Resources/shaders/SpritePS.hlsl",//シェーダファイル名
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
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
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
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
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
	CD3DX12_DESCRIPTOR_RANGE  descRangeSRV;


	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	//t0 レジスタ
	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);	//定数バッファビューとして初期化(b0)
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
