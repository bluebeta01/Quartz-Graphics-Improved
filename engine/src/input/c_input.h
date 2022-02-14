#pragma once
#include <vector>

namespace Quartz
{
class Input
{
public:
	static void initialize();
	static void update();
	static void registerKeyDown(char key);
	static void registerKeyUp(char key);
	static void registerMouseButtonDown(int button);
	static void registerMouseButtonUp(int button);
	static bool isKeyDown(char key);
	static bool isKeyDownThisFrame(char key);
	static bool isMouseButtonDown(int button);
	static bool isMouseButtonDownThisFrame(int button);
private:
	static std::vector<char> m_keysDown;
	static std::vector<char> m_keysDownThisFrame;
	static std::vector<int> m_mouseButtonsDown;
	static std::vector<int> m_mouseButtonsDownThisFrame;
	Input();
};
}