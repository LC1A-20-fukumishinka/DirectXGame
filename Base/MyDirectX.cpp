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
	//�o�b�N�o�b�t�@�̔ԍ����擾(2�K�v�Ȃ̂�0�Ԃ�1��)
	bbIndex =swapchain->GetCurrentBackBufferIndex();

	//�@���\�[�X�o���A�ŏ������݉\�ɕύX
	//�\����ԋ�`���ԂɕύX
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));
#pragma endregion

#pragma region SetRenderTarget
	//�A�`���w��
//�����_�[�^�[�Q�b�g�r���[�p
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
		bbIndex,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		0,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

#pragma endregion

	//��ʃN���A
	ClearRenderTarget();
	//�[�x�n���Z�b�g
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
	//�`���Ԃ���\����ԂɕύX����
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));
#pragma endregion

#pragma region CloseCommand
	//���߂̃N���[�Y
	cmdList->Close();
#pragma endregion

#pragma region ExecuteCommand
	//�R�}���h���X�g�̎��s
	ID3D12CommandList *cmdLists[] = { cmdList.Get() };//�R�}���h���X�g�̔z��
	cmdQueue->ExecuteCommandLists(1, cmdLists);
#pragma endregion

#pragma region Screenflip
	//�o�b�t�@���t���b�v(���\�̓���ւ�)
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
	cmdAllocator->Reset();//�L���[���N���A
	cmdList->Reset(cmdAllocator.Get(), nullptr);//�ĂуR�}���h���X�g�����߂鏀��
#pragma endregion


}

void MyDirectX::ClearRenderTarget()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
		bbIndex,
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	float clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };//���ۂ��F
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
	//DXGI�t�@�N�g���[�̐���
	HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
#pragma endregion

	if (FAILED(result))
	{
		assert(0);
		return false;
	}
#pragma region graphicAdapter
	//�A�_�v�^�[�̗񋓗p
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	//ComPtr<IDXGIAdapter1> tmpAdapter;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> tmpAdapter;

	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter);//���I�z��ɒǉ�����
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);//�A�_�v�^�[�̏����擾

		//�\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		std::wstring strDesc = adesc.Description;//�A�_�v�^�[��
		//Inter UHD Graphics (�I���{�[�h���O���t�B�b�N) �����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //�̗p
			break;
		}
	}
#pragma endregion

#pragma region device
	//�Ή����x���̔z��
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
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//�f�o�C�X�𐶐��o�������_�Ń��[�v�𔲂���
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
	//�R�}���h�A���P�[�^�𐶐�
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
	//�R�}���h���X�g�𐶐�
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
	//�W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
#pragma endregion

	return true;
}

bool MyDirectX::CreateSwapChain()
{
	WinAPI *Win = WinAPI::GetInstance();

	//winAPI�I�u�W�F�N�g���K�v
#pragma region swapChain
	//�e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�F���̏���
	swapchainDesc.SampleDesc.Count = 1;//�}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//�o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2;//�o�b�t�@����2�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//�t���b�v��͔j��
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
	//��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	swapchain1.As(&swapchain);
#pragma endregion

	
	return true;
}

bool MyDirectX::CreateFinalRenderTargets()
{
#pragma region RTV
	//�e��ݒ�����ĂŃX�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = 2;//���\��2��
	dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeaps));

	//���\��2���ɂ���
	this->backBuffers.resize(2);
	for (int i = 0; i < 2; i++)
	{
		//�X���b�v�`�F�[������o�b�t�@���擾
		HRESULT result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(result))
		{
			assert(0);
			return false;
		}
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			i,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type));
		//�����_�[�^�[�Q�b�g�r���[�̐���
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
	//���\�[�X�ݒ�
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

	//���\�[�X�̐���
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
	//�[�x�r���[�p�f�X�N���v�^�q�[�v����
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
	//�[�x�r���[�쐬
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
	//�t�F���X�̐���
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
