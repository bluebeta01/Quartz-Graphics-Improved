#pragma once
#include <Windows.h>

namespace Quartz
{
class Engine
{
public:
	Engine();
	void initialize(bool editorMode, HWND windowHandle = nullptr);
	void gameLoopBody();
private:
	bool m_editorMode;
	void updateSystems();
	void initializeSystems(HWND windowHandle);
	void initGameLoop();
};
}