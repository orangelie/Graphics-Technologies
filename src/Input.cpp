#include "Global.h"
#include "Input.h"

void Input::Init(HWND hwnd)
{
    // 키보드
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);

    // 마우스
    HINSTANCE hInst = GetModuleHandle(0);

    DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&_directInput, 0);
    _directInput->CreateDevice(GUID_SysMouse, &_mouseDevice, 0);

    _mouseDevice->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    _mouseDevice->SetDataFormat(&c_dfDIMouse);
    _mouseDevice->Acquire();

    _states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update()
{
    // 키보드
	HWND hwnd = ::GetActiveWindow();
	if (_hwnd != hwnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			_states[key] = KEY_STATE::NONE;

		return;
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// 키가 눌려 있으면 true
		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);

    // 마우스
    HRESULT result = _mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &_diMouseState);
    if (FAILED(result)) {
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            _mouseDevice->Acquire();
        }
        else {
            exit(-1);
            return;
        }
    }
}
