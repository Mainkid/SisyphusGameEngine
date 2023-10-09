#include "InputSystem.h"

SyResult InputSystem::Init()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Input system initialization successful. ");
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
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Input system destruction successful. ");
	return SyResult();
}

void InputSystem::ProcessInput()
{
}
