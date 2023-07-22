#pragma once
#include "WinInput.h"
#include "EngineCore.h"

WinInput::WinInput()
{

	windowHWND = EngineCore::instance()->window->GetHWND();
	//inputDevice = new InputDevice(_game);

	keys = new std::unordered_set<Keys>();

	if (rawInputInitialized == false)
	{
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			std::cout << "Raw input device registration failed!\n";
		}
		rawInputInitialized = true;
	}
}

WinInput::~WinInput()
{
	delete keys;
}

void WinInput::AddPressedKey(Keys key)
{
	keys->insert(key);
}

void WinInput::RemovePressedKey(Keys key)
{
	keys->erase(key);
}

bool WinInput::IsKeyDown(Keys key)
{
	return keys->count(key);
}

void WinInput::OnMouseMove(int x, int y)
{
	prevPos.x = mousePos.x;
	prevPos.y = mousePos.y;

	mousePos.x = x;
	mousePos.y = y;

	rawOffset.x = mousePos.x - prevPos.x;
	rawOffset.y = mousePos.y - prevPos.y;


}

bool WinInput::ProcessMessages()
{

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (PeekMessage(&msg, EngineCore::instance()->window->GetHWND(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);


		switch (msg.message)
		{


		case WM_KEYDOWN:
		{
			AddPressedKey((Keys)msg.wParam);
			break;
		}
		case WM_KEYUP:
		{

			RemovePressedKey((Keys)msg.wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			AddPressedKey(Keys::LeftButton);

			break;
		}
		case WM_RBUTTONDOWN:
		{
			AddPressedKey(Keys::RightButton);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			AddPressedKey(Keys::MiddleButton);
			break;
		}
		case WM_LBUTTONUP:
		{
			RemovePressedKey(Keys::LeftButton);
			break;
		}
		case WM_RBUTTONUP:
		{
			RemovePressedKey(Keys::RightButton);
			break;
		}
		case WM_MBUTTONUP:
		{
			RemovePressedKey(Keys::MiddleButton);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			int x = LOWORD(msg.lParam);
			int y = HIWORD(msg.lParam);
			if (GET_WHEEL_DELTA_WPARAM(msg.wParam) > 0)
			{
				this->WheelMove.Broadcast(Keys::WheelUp);
			}
			else if (GET_WHEEL_DELTA_WPARAM(msg.wParam) < 0)
			{
				this->WheelMove.Broadcast(Keys::WheelDown);
			}
			break;
		}

		case WM_INPUT:
		{
			UINT dataSize;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(msg.lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);

				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(msg.lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());

					if (raw->header.dwType == RIM_TYPEMOUSE)
					{

						rawOffset.x = raw->data.mouse.lLastX;
						rawOffset.y = raw->data.mouse.lLastY;
						RawOffsetEvent.Broadcast(rawOffset);
					}
				}
			}

			break;
		}



		}
	}
	//DefWindowProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);

	return true;
}

void WinInput::GetInput()
{
	//Input of ImGUI
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	rawOffset.x = 0;
	rawOffset.y = 0;
}

POINT WinInput::GetMousePos()
{
	return mousePos;
}

POINT WinInput::GetRawOffset()
{
	return rawOffset;
}