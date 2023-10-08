#include "DisplayWin32.h"
#include "EngineCore.h"
#include "ServiceLocator.h"

static bool isInitialized = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
		return true;
	UINT g_ResizeWidth;
	UINT g_ResizeHeight;
	switch (umessage)
	{
	case WM_SIZE:
		if (wparam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lparam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lparam);
		//if (isInitialized)
		//{
		//	EngineCore::instance()->window->HandleResize(g_ResizeWidth, g_ResizeHeight);
		//	ImGui::GetIO().DisplaySize = ImVec2(g_ResizeWidth, g_ResizeHeight);
		//}
		//else
		//	isInitialized = true;
		
		
		return 0;
	case WM_SYSCOMMAND:
		if ((wparam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		ServiceLocator::instance()->Get<EngineContext>()->isClosed = true;
		::PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lparam;
			::SetWindowPos(hwnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProcW(hwnd, umessage, wparam, lparam);
}

DisplayWin32::DisplayWin32(LPCWSTR applicationName, HINSTANCE hInstance,const int& screenWidth,const int& screenHeight)
{
	wc = { sizeof(wc), CS_CLASSDC, HandleMessageSetup, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);

	//wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//wc.lpfnWndProc = HandleMessageSetup;
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = hInstance;
	//wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	//wc.hIconSm = wc.hIcon;
	//wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	//wc.lpszMenuName = nullptr;
	//wc.lpszClassName = applicationName;
	//wc.cbSize = sizeof(WNDCLASSEX);

	//// Register the window class.
	//RegisterClassEx(&wc);

	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	/*hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);*/

	HWND _hwnd = ::CreateWindowW(wc.lpszClassName,
		L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, nullptr, nullptr, wc.hInstance, nullptr);

	this->hWnd = _hwnd;

	
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(true);
	clientHeight = screenHeight;
	clientWidth = screenWidth;
}

DisplayWin32::~DisplayWin32()
{

}

HWND DisplayWin32::GetHWND()
{
	return hWnd;
}

float DisplayWin32::GetHeight()
{
	return clientHeight;
}

float DisplayWin32::GetWidth()
{
	return clientWidth;
}

void DisplayWin32::HandleResize(int width, int height)
{
	this->clientHeight = height;
	this->clientWidth = width;
}
