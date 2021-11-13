#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

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
	bool Trigger(int KeyCode);
	bool Release(int KeyCode);

	static Input *GetInstance();
private:
	IDirectInput8 *dinput;
	IDirectInputDevice8 *devkeyboard;
	BYTE key[256];
	BYTE oldkey[256];

	//ˆê“I‚É‚½‚¹‚Ä‚İ‚é
	HRESULT result;
};