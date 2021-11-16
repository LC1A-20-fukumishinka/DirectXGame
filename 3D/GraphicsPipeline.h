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
class GraphicsPipeline
{
private:
	GraphicsPipeline();
	~GraphicsPipeline();
public:
	GraphicsPipeline(GraphicsPipeline &obj) = delete;
	GraphicsPipeline &operator=(const GraphicsPipeline &obj) = delete;

	void Object3DCreateGraphPipeline();

	const PipeClass::PipelineSet &GetPipeLine();

	static GraphicsPipeline *GetInstance();
private:
	PipeClass::PipelineSet pipelineSet;
};