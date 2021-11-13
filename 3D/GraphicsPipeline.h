#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class GraphicsPipeline
{
public:
	void Object3DCreateGraphPipeline(ID3D12Device *dev);
public://サブクラス
	struct PipelineSet
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	//ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;	//パイプラインステート
	};

private:
	PipelineSet pipelineSet;
};