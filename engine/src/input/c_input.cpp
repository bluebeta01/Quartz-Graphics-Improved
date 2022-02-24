#include "pch.h"
#include "input/c_input.h"

namespace Quartz
{
std::vector<char> Input::m_keysDown;
std::vector<char> Input::m_keysDownThisFrame;
std::vector<int> Input::m_mouseButtonsDown;
std::vector<int> Input::m_mouseButtonsDownThisFrame;
int Input::m_cursorX;
int Input::m_cursorY;

Input::Input()
{

}

void Input::initialize()
{

}

void Input::update()
{
	m_keysDownThisFrame.clear();
	m_mouseButtonsDownThisFrame.clear();
}

void Input::registerKeyDown(char key)
{
	if (!isKeyDown(key) && !isKeyDownThisFrame(key))
	{
		m_keysDown.push_back(key);
		m_keysDownThisFrame.push_back(key);
	}
}

void Input::registerKeyUp(char key)
{
	for (int i = 0; i < m_keysDown.size(); i++)
	{
		if (m_keysDown[i] == key)
			m_keysDown.erase(m_keysDown.begin() + i);
	}
}

void Input::registerMouseButtonDown(int button)
{
	if (!isMouseButtonDown(button) && !isMouseButtonDownThisFrame(button))
	{
		m_mouseButtonsDown.push_back(button);
		m_mouseButtonsDownThisFrame.push_back(button);
	}
}

void Input::registerMouseButtonUp(int button)
{
	for (int i = 0; i < m_mouseButtonsDown.size(); i++)
	{
		if (m_mouseButtonsDown[i] == button)
			m_mouseButtonsDown.erase(m_mouseButtonsDown.begin() + i);
	}
}
void Input::registerCursorPosition(int x, int y)
{
	m_cursorX = x;
	m_cursorY = y;
}
bool Input::isKeyDown(char key)
{
	for (char c : m_keysDown)
		if (c == key)
			return true;
	return false;
}
bool Input::isKeyDownThisFrame(char key)
{
	for (char c : m_keysDownThisFrame)
		if (c == key)
			return true;
	return false;
}
bool Input::isMouseButtonDown(int button)
{
	for (int b : m_mouseButtonsDown)
		if (button == b)
			return true;
	return false;
}
bool Input::isMouseButtonDownThisFrame(int button)
{
	for (int b : m_mouseButtonsDownThisFrame)
	{
		if (b == button)
			return true;
	}
	return false;
}
int Input::getCursorX()
{
	return m_cursorX;
}
int Input::getCursorY()
{
	return m_cursorY;
}
}