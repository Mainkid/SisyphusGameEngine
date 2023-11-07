#include "InputSystem.h"

SyResult InputSystem::Init()
{
	return SyResult();
}

SyResult InputSystem::Run()
{
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return SyResult();
}

SyResult InputSystem::Destroy()
{
	return SyResult();
}

void InputSystem::ProcessInput()
{
}
