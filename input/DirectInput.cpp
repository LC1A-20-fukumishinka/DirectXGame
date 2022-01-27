#include "DirectInput.h"
#include <cassert>
#pragma comment(lib, "Xinput.lib")

Input::Input()
{
}

Input::~Input()
{
}

void Input::Init(WNDCLASSEX &w, HWND &hwnd)
{
	HRESULT result;
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


	for (int i = 0; i < 4; i++)
	{
		oldState[i] = padState[i];

		ZeroMemory(&padState[i], sizeof(XINPUT_STATE));


		dwResult[i] = XInputGetState(static_cast<DWORD>(i), &padState[i]);
	}

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

bool Input::Button(WORD bitmask, int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if (dwResult[controllNum] == ERROR_SUCCESS)
	{
		bool isButton = padState[controllNum].Gamepad.wButtons & bitmask;;
		return isButton;
	}
	else
	{
		return false;
	}
}

bool Input::ButtonTrigger(WORD bitmask, int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if (dwResult[controllNum] == ERROR_SUCCESS)
	{
		bool isButton = padState[controllNum].Gamepad.wButtons & bitmask;
		bool isOldButton = oldState[controllNum].Gamepad.wButtons & bitmask;
		return (isButton && !isOldButton);
	}
	else
	{
		return false;
	}
}

bool Input::ButtonRelease(WORD bitmask, int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if (dwResult[controllNum] == ERROR_SUCCESS)
	{
		bool isButton = padState[controllNum].Gamepad.wButtons & bitmask;
		bool isOldButton = oldState[controllNum].Gamepad.wButtons & bitmask;
		return (!isButton && isOldButton);
	}
	else
	{
		return false;
	}
}

DirectX::XMFLOAT2 Input::LStick(int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if ((padState[controllNum].Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		padState[controllNum].Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(padState[controllNum].Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			padState[controllNum].Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		lStick[controllNum] = { 0, 0 };
	}
	else
	{
		float stickMax = 32767;
		lStick[controllNum] = { padState[controllNum].Gamepad.sThumbLX / stickMax , padState[controllNum].Gamepad.sThumbLY / stickMax };
	}

	return lStick[controllNum];
}

DirectX::XMFLOAT2 Input::RStick(int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if ((padState[controllNum].Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		padState[controllNum].Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(padState[controllNum].Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			padState[controllNum].Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		rStick[controllNum] = { 0, 0 };
	}
	else
	{
		float stickMax = 32767;
		rStick[controllNum] = { padState[controllNum].Gamepad.sThumbRX / stickMax , padState[controllNum].Gamepad.sThumbRY / stickMax };
	}

	return rStick[controllNum];
}

float Input::LTrigger(int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if (padState[controllNum].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		float TriggerInput = padState[controllNum].Gamepad.bLeftTrigger / 255.0f;
		return TriggerInput;
	}
	return 0.0f;
}

float Input::RTrigger(int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if (padState[controllNum].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		float TriggerInput = padState[controllNum].Gamepad.bRightTrigger / 255.0f;
		return TriggerInput;
	}
	return 0.0f;

}

Input *Input::GetInstance()
{
	static Input instance;
	return &instance;
}

bool Input::isPadConnect(int controllNum)
{
	if (!Check(controllNum))
	{
		assert(0);
	}
	if (dwResult[controllNum] == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Input::Check(int controllNum)
{
	return (controllNum >= 0 && controllNum < 4);
}
