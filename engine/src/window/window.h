#pragma once
#include <Windows.h>

namespace Quartz
{
namespace Window
{
extern HWND windowHandle;
extern void initialize(int width, int height, HWND hwnd = nullptr);
extern void update();
}
}