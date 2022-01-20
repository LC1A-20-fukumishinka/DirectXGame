#include "WinAPI.h"
#include "MyDirectX.h"
WinAPI::WinAPI()
{
}

void WinAPI::msgCheck()
{
	//���b�Z�[�W������H
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);//�L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&msg);//�vr�V�[����Ƀ��b�Z�[�W�𑗂�
	}
}

bool WinAPI::loopBreak()
{
	//�~�{�^���ŏC�����b�Z�[�W��������Q�[�����[�v�𔲂���
	if (msg.message == WM_QUIT)
	{
		return false;
	}

	return true;
}

void WinAPI::end()
{
	UnregisterClass(w.lpszClassName, w.hInstance);
}

void WinAPI::Init(const int winWidth, const int winHeight)
{
	this->winWidth = winWidth;
	this->winHeight = winHeight;



	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//�E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectXGame";//�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr);//�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//�J�[�\���̎w��

	//�E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&w);

	//�E�B���h�E�T�C�Y{ X���W, Y���W, ����, �c��}
	RECT wrc = { 0,0,this->winWidth, this->winHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�����ŃT�C�Y�␳

	hwnd = CreateWindow(w.lpszClassName,		//�N���X��
		L"�X�g���{",							//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,					//�W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,							//�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,							//�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,					//�E�B���h�E����
		wrc.bottom - wrc.top,					//�E�B���h�E�c��
		nullptr,								//�e�E�B���h�E�n���h��
		nullptr,								//���j���[�n���h��
		w.hInstance,							//�Ăяo���A�v���P�[�V����
		nullptr);								//�I�v�V����

	//�E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	MyDirectX *instance = MyDirectX::GetInstance();

	instance->Init();
}

WinAPI *WinAPI::GetInstance()
{
	static WinAPI instance;
	return &instance;
}

WinAPI::~WinAPI()
{
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY://�E�B���h�E���j�����ꂽ
		PostQuitMessage(0);//OS�ɑ΂��āA�A�v���̏C����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�W���̏������s��
}