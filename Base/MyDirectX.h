#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxgidebug.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class MyDirectX
{
private:
	MyDirectX();
	~MyDirectX();
public:
	MyDirectX(MyDirectX &obj) = delete;
	MyDirectX &operator=(const MyDirectX &obj) = delete;
	int winWidth, winHeight;

	static MyDirectX *GetInstance();
	void Init();
	void PreDraw();
	void PostDraw();


	void ClearRenderTarget();

	void ClearDepthBuffer();

public://Getter
	ID3D12Device *GetDevice();
	ID3D12GraphicsCommandList *GetCommandList();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvH();
private:
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	Microsoft::WRL::ComPtr <ID3D12Device> dev;
	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain1;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	Microsoft::WRL::ComPtr<ID3D12Resource>depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	UINT bbIndex;
	UINT64 fenceVal;

	Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
private://各種項目の初期化処理
	//DXGIデバイスの初期化
	bool InitializeDXGIDevice();

	//コマンド関連の初期化
	bool InitializeCommand();

	//スワップチェーンの生成
	bool CreateSwapChain();

	//コマンド関連初期化
	bool CreateFinalRenderTargets();

	//レンダーターゲット生成
	bool CreateDepthBuffer();

	//フェンス生成
	bool CreateFence();
};