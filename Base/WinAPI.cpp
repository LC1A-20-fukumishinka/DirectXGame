#include "WinAPI.h"
#include "MyDirectX.h"
WinAPI::WinAPI()
{
}

void WinAPI::msgCheck()
{
	//メッセージがある？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);//キー入力メッセージの処理
		DispatchMessage(&msg);//プrシーじゃにメッセージを送る
	}
}

bool WinAPI::loopBreak()
{
	//×ボタンで修了メッセージが来たらゲームループを抜ける
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
	w.lpfnWndProc = (WNDPROC)WindowProc;//ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame";//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);//ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);//カーソルの指定

	//ウィンドウクラスをOSに登録
	RegisterClassEx(&w);

	//ウィンドウサイズ{ X座標, Y座標, 横幅, 縦幅}
	RECT wrc = { 0,0,this->winWidth, this->winHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//自動でサイズ補正

	hwnd = CreateWindow(w.lpszClassName,		//クラス名
		L"ストロボ",							//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,					//標準的なウィンドウスタイル
		CW_USEDEFAULT,							//表示X座標(OSに任せる)
		CW_USEDEFAULT,							//表示Y座標(OSに任せる)
		wrc.right - wrc.left,					//ウィンドウ横幅
		wrc.bottom - wrc.top,					//ウィンドウ縦幅
		nullptr,								//親ウィンドウハンドル
		nullptr,								//メニューハンドル
		w.hInstance,							//呼び出しアプリケーション
		nullptr);								//オプション

	//ウィンドウ表示
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
	//メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY://ウィンドウが破棄された
		PostQuitMessage(0);//OSに対して、アプリの修了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//標準の処理を行う
}