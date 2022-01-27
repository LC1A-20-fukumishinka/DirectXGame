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

	bool Button(WORD bitmask, int controllNum = 0);
	bool ButtonTrigger(WORD bitmask, int controllNum = 0);
	bool ButtonRelease(WORD bitmask, int controllNum = 0);
	DirectX::XMFLOAT2 LStick(int controllNum = 0);
	DirectX::XMFLOAT2 RStick(int controllNum = 0);
	float LTrigger(int controllNum = 0);
	float RTrigger(int controllNum = 0);
	static Input *GetInstance();

	bool isPadConnect(int controllNum = 0);

	bool Check(int controllNum = 0);
private:
	IDirectInput8 *dinput;
	IDirectInputDevice8 *devkeyboard;
	BYTE key[256];
	BYTE oldkey[256];

	//àÍéûìIÇ…éùÇΩÇπÇƒÇ›ÇÈ
	DWORD dwResult[4];
	XINPUT_STATE padState[4];
	XINPUT_STATE oldState[4];
	DirectX::XMFLOAT2 lStick[4];
	DirectX::XMFLOAT2 rStick[4];
};