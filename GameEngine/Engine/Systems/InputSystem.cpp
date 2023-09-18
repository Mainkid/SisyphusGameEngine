#include "InputSystem.h"

void InputSystem::Init()
{
}

void InputSystem::Run()
{
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void InputSystem::Destroy()
{
}

void InputSystem::ProcessInput()
{
}
