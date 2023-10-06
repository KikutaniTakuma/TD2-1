#include "WinApp.h"
#pragma comment(lib, "winmm.lib")
#include <cassert>
#include "externals/imgui/imgui_impl_win32.h"
#include "Utils/Math/Vector2.h"

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WinApp::WinApp():
	hwnd{},
	w{},
	windowStyle(0u),
	windowRect{},
	windowName()
{
	timeBeginPeriod(1);
}

WinApp::~WinApp() {
	UnregisterClass(w.lpszClassName, w.hInstance);
}

LRESULT WinApp::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:        // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

void WinApp::Create(const std::wstring& windowTitle, int32_t width, int32_t height) {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowName = windowTitle;

	// 最大化ボタンを持たないかつサイズ変更不可
	windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = WindowProcedure;
	w.lpszClassName = windowTitle.c_str();
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&w);

	windowRect = { 0,0,width, height };

	// 指定のサイズになるようなウィンドウサイズを計算
	AdjustWindowRect(&windowRect, windowStyle, false);

	hwnd = CreateWindow(
		w.lpszClassName,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr
	);

	windowStyle &= ~WS_THICKFRAME;

	SetWindowLong(hwnd, GWL_STYLE, windowStyle);
	SetWindowPos(
		hwnd, NULL, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
	ShowWindow(hwnd, SW_NORMAL);
}


Vector2 WinApp::GetWindowSize() const {
	return Vector2(float(windowRect.right), float(windowRect.bottom));
}