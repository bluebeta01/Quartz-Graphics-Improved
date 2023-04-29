#pragma once
#include <Windows.h>
#include <functional>
#include <memory>

namespace Quartz
{
namespace Window
{
extern HWND windowHandle;
extern MSG msg;
extern WPARAM wParam;
extern LPARAM lParam;
extern UINT message;
extern void initialize(int width, int height, HWND hwnd = nullptr);
extern void update();
extern std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)> windowProcCallback;
}
}