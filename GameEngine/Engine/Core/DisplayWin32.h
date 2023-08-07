#pragma once
#include <windows.h>
#include <WinUser.h>
#include <iostream>
#include "../../vendor/ImGui/imgui_impl_win32.h"
#include "../../vendor/ImGui/imgui.h"
#include "../../vendor/ImGui/imgui_impl_dx11.h"




class EngineCore;

class DisplayWin32
{
public:
	DisplayWin32() {};
	DisplayWin32( LPCWSTR applicationName, HINSTANCE hInstance,
		const int& screenHeight = 800, const int& screenWidth = 800);

	~DisplayWin32();
	HWND GetHWND();
	float GetHeight();
	float GetWidth();
	void HandleResize(int width, int height);

private:
	
	float clientHeight;
	float clientWidth;
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASSEXW wc;

};

