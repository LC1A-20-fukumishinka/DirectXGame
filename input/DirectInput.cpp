#include "DirectInput.h"

#pragma comment(lib, "Xinput.lib")

Input::Input()
{
}

Input::~Input()
{
}

void Input::Init(WNDCLASSEX &w, HWND &hwnd)
{
	for (int i = 0; i < 256; i++)
	{
		key[i] = 0;
		oldkey[i] = 0;
	}
	int a = XINPUT_GAMEPAD_A;
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dinput, nullptr);

	devkeyboard = nullptr;
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);	//標準形式

	result = devkeyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

}

void Input::Update()
{
	for (int i = 0; i < 256; i++)
	{
		oldkey[i] = key[i];
	}
	HRESULT result;
	result = devkeyboard->Acquire();//入力受け付けますよの処理

	result = devkeyboard->GetDeviceState(sizeof(key), key);//全部のキー入力の状態を取得する



	oldState = padState;
	ZeroMemory(&padState, sizeof(XINPUT_STATE));

	dwResult = XInputGetState(static_cast<DWORD>(0), &padState);


}

bool Input::Key(int KeyCode)
{
	if (key[KeyCode] == 0x80)return true;
	return false;
}

bool Input::KeyTrigger(int KeyCode)
{
	if ((key[KeyCode] == 0x80) && (oldkey[KeyCode] == 0x00))return true;
	return false;
}

bool Input::KeyRelease(int KeyCode)
{
	if ((key[KeyCode] == 0x00) && (oldkey[KeyCode] == 0x80))return true;
	return false;
}

bool Input::Button(WORD bitmask)
{
	if (dwResult == ERROR_SUCCESS)
	{
		bool isButton = padState.Gamepad.wButtons & bitmask;;
		return isButton;
	}
	else
	{
		return false;
	}
}

bool Input::ButtonTrigger(WORD bitmask)
{
	if (dwResult == ERROR_SUCCESS)
	{
		bool isButton = padState.Gamepad.wButtons & bitmask;
		bool isOldButton = oldState.Gamepad.wButtons & bitmask;
		return (isButton && !isOldButton);
	}
	else
	{
		return false;
	}
}

bool Input::ButtonRelease(WORD bitmask)
{
	if (dwResult == ERROR_SUCCESS)
	{
		bool isButton = padState.Gamepad.wButtons & bitmask;
		bool isOldButton = oldState.Gamepad.wButtons & bitmask;
		return (!isButton && isOldButton);
	}
	else
	{
		return false;
	}
}

DirectX::XMFLOAT2 Input::LStick()
{
	if ((padState.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		padState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(padState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			padState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		lStick = { 0, 0 };
	}
	else
	{
		float stickMax = 32767;
		lStick = { padState.Gamepad.sThumbLX / stickMax , padState.Gamepad.sThumbLY / stickMax };
	}

	return lStick;
}

DirectX::XMFLOAT2 Input::RStick()
{
	if ((padState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		padState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(padState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			padState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		rStick = { 0, 0 };
	}
	else
	{
		float stickMax = 32767;
		rStick = { padState.Gamepad.sThumbRX / stickMax , padState.Gamepad.sThumbRY / stickMax };
	}

	return rStick;
}

float Input::LTrigger()
{
	if (padState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		float TriggerInput = padState.Gamepad.bLeftTrigger/ 255.0f;
		return TriggerInput;
	}
	return 0.0f;
}

float Input::RTrigger()
{
	if (padState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		float TriggerInput = padState.Gamepad.bRightTrigger / 255.0f;
		return TriggerInput;
	}
	return 0.0f;

}

Input *Input::GetInstance()
{
	static Input instance;
	return &instance;
}
