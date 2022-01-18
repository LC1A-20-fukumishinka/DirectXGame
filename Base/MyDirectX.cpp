#include "MyDirectX.h"
#include "WinAPI.h"
#include <vector>
#include <string>
#include <cassert>
MyDirectX::MyDirectX()
{
}

MyDirectX::~MyDirectX()
{
}

MyDirectX *MyDirectX::GetInstance()
{
	static MyDirectX instance;
	return &instance;
}

void MyDirectX::Init()
{
	WinAPI *Win = WinAPI::GetInstance();
	this->winWidth = Win->winWidth;
	this->winHeight = Win->winHeight;
	HRESULT result;
#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	if (!InitializeDXGIDevice())
	{
		assert(0);
	}

	if (!InitializeCommand())
	{
		assert(0);
	}

	if (!CreateSwapChain())
	{
		assert(0);
	}

	if (!CreateFinalRenderTargets())
	{
		assert(0);
	}

	if (!CreateDepthBuffer())
	{
		assert(0);
	}

	if (!CreateFence())
	{
		assert(0);
	}
}

void MyDirectX::PreDraw()
{
#pragma region ChangeResourceBarrier
	//バックバッファの番号を取得(2つ必要なので0番か1番)
	bbIndex =swapchain->GetCurrentBackBufferIndex();

	//①リソースバリアで書き込み可能に変更
	//表示状態空描画状態に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));
#pragma endregion

#pragma region SetRenderTarget
	//②描画先指定
//レンダーターゲットビュー用
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
		bbIndex,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

#pragma endregion

	//画面クリア
	ClearRenderTarget();
	//深度地リセット
	ClearDepthBuffer();

#pragma region Viewport
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, winWidth, winHeight));
#pragma endregion

#pragma region Scissor
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, winWidth, winHeight));
#pragma endregion

}

void MyDirectX::PostDraw()
{
#pragma region ChangeResourceBArrier
	//描画状態から表示状態に変更する
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));
#pragma endregion

#pragma region CloseCommand
	//命令のクローズ
	cmdList->Close();
#pragma endregion

#pragma region ExecuteCommand
	//コマンドリストの実行
	ID3D12CommandList *cmdLists[] = { cmdList.Get() };//コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);
#pragma endregion

#pragma region Screenflip
	//バッファをフリップ(裏表の入れ替え)
	swapchain->Present(1, 0);
#pragma endregion

#pragma region ExecuteCommandWait
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
#pragma endregion

#pragma region CommandReset
	cmdAllocator->Reset();//キューをクリア
	cmdList->Reset(cmdAllocator.Get(), nullptr);//再びコマンドリストをためる準備
#pragma endregion


}

void MyDirectX::ClearRenderTarget()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
		bbIndex,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	float clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };//青っぽい色
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
}

void MyDirectX::ClearDepthBuffer()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));

	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

ID3D12Device *MyDirectX::GetDevice()
{
	return dev.Get();
}

ID3D12GraphicsCommandList *MyDirectX::GetCommandList()
{
	return cmdList.Get();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE MyDirectX::GetDsvH()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE
		(dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
}

bool MyDirectX::InitializeDXGIDevice()
{
#pragma region DXGIFactory
	//DXGIファクトリーの生成
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
#pragma endregion

	if (FAILED(result))
	{
		assert(0);
		return false;
	}
#pragma region graphicAdapter
	//アダプターの列挙用
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	//ComPtr<IDXGIAdapter1> tmpAdapter;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter;

	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter);//動的配列に追加する
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);//アダプターの情報を取得

		//ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		std::wstring strDesc = adesc.Description;//アダプター名
		//Inter UHD Graphics (オンボード尾グラフィック) を回避
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //採用
			break;
		}
	}
#pragma endregion

#pragma region device
	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//デバイスを生成出来た時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion

	if (FAILED(result))
	{
		assert(0);
		return false;
	}
	return true;
}

bool MyDirectX::InitializeCommand()
{
#pragma region commandAllocator
	//コマンドアロケータを生成
	HRESULT result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
#pragma endregion
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
#pragma region commandList
	//コマンドリストを生成
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));
#pragma endregion
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
#pragma region commandQueue
	//標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
#pragma endregion

	return true;
}

bool MyDirectX::CreateSwapChain()
{
	WinAPI *Win = WinAPI::GetInstance();

	//winAPIオブジェクトが必要
#pragma region swapChain
	//各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色情報の書式
	swapchainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//バックバッファ用
	swapchainDesc.BufferCount = 2;//バッファ数を2つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		Win->hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);


	if (FAILED(result))
	{
		assert(0);
		return false;
	}
	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapchain1.As(&swapchain);
#pragma endregion

	
	return true;
}

bool MyDirectX::CreateFinalRenderTargets()
{
#pragma region RTV
	//各種設定をしてでスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビュー
	heapDesc.NumDescriptors = 2;//裏表の2つ
	dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeaps));

	//裏表の2つ分について
	this->backBuffers.resize(2);
	for (int i = 0; i < 2; i++)
	{
		//スワップチェーンからバッファを取得
		HRESULT result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(result))
		{
			assert(0);
			return false;
		}
		//デスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			i,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
		//レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);
	}
#pragma endregion

	return true;
}

bool MyDirectX::CreateDepthBuffer()
{
#pragma region depthBuffer
#pragma region depthResDescSet
	//リソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			this->winWidth,
			this->winHeight,
			1,
			0,
			1,
			0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
#pragma endregion


#pragma region depthBuff

	//リソースの生成
	HRESULT result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);
#pragma endregion
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
#pragma endregion

#pragma region DBV
#pragma region dsvHeapDesc
	//深度ビュー用デスクリプタヒープ制作
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
#pragma endregion
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
#pragma region dsvDesc
	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion
#pragma endregion
	return true;
}

bool MyDirectX::CreateFence()
{
#pragma region fence
	//フェンスの生成
	fenceVal = 0;

	HRESULT result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

#pragma endregion
	if (FAILED(result))
	{
		assert(0);
		return false;
	}
	return true;
}
