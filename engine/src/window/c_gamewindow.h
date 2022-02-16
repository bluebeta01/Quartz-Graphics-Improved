#pragma once
#include <Windows.h>

namespace Quartz
{
class GameWindow
{
public:
	static void initialize(HWND windowHandle = nullptr, int width = 1920, int height = 1080);
	static void update();
	static HWND* getHandle();
	static int getWidth();
	static int getHeight();
	static void setWidth(int width);
	static void setHeight(int height);
	static void setResizedFlag();
	static bool getReiszedFlag();
	static void setTerminated(bool terminated);
	static bool getTerminated();
private:
	static int s_width;
	static int s_height;
	static bool s_resizedFlag;
	static bool s_terminated;
	GameWindow();
};
}