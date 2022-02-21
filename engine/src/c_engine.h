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
	void updateSystems();
private:
	bool m_editorMode;
	void initializeSystems(HWND windowHandle);
	void initGameLoop();
};
}