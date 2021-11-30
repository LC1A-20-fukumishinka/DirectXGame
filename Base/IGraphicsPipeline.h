#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
namespace PipeClass
{
	struct PipelineSet
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;	//ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;	//パイプラインステート
	};
}
class IGraphicsPipeline
{
protected:
	IGraphicsPipeline() {};
	virtual ~IGraphicsPipeline() {};
public:

	IGraphicsPipeline(IGraphicsPipeline &obj) = delete;
	IGraphicsPipeline &operator=(const IGraphicsPipeline &obj) = delete;

	virtual const PipeClass::PipelineSet &GetPipeLine() = 0;

};