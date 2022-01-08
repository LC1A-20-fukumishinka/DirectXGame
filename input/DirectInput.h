#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#include <DirectXMath.h>
class Input
{
private:
	Input();
	~Input();
public:

	Input(const Input &obj) = delete;
	Input &operator=(const Input &obj) = delete;
	void Init(WNDCLASSEX &w, HWND &hwnd);
	void Update();

	bool Key(int KeyCode);
	bool KeyTrigger(int KeyCode);
	bool KeyRelease(int KeyCode);

	bool Button(WORD bitmask);
	DirectX::XMFLOAT2 LStick();
	DirectX::XMFLOAT2 RStick();
	float LTrigger();
	float RTrigger();
	static Input *GetInstance();
private:
	IDirectInput8 *dinput;
	IDirectInputDevice8 *devkeyboard;
	BYTE key[256];
	BYTE oldkey[256];

	//àÍéûìIÇ…éùÇΩÇπÇƒÇ›ÇÈ
	HRESULT result;
	DWORD dwResult;
	XINPUT_STATE padState;
	DirectX::XMFLOAT2 lStick;
	DirectX::XMFLOAT2 rStick;
};