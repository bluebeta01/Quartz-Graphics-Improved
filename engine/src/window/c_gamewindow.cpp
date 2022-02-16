#include "pch.h"
#include "window/c_gamewindow.h"
#include "window/window.h"

namespace Quartz
{

int GameWindow::s_height = 0;
int GameWindow::s_width = 0;
bool GameWindow::s_resizedFlag = false;
bool GameWindow::s_terminated = false;

GameWindow::GameWindow()
{

}

void GameWindow::initialize(HWND windowHandle, int width, int height)
{
	s_width = width;
	s_height = height;
	Window::initialize(width, height, windowHandle);
}

void GameWindow::update()
{
	s_resizedFlag = false;
	Window::update();
}
HWND* GameWindow::getHandle()
{
	return &Window::windowHandle;
}
int GameWindow::getWidth()
{
	return s_width;
}
int GameWindow::getHeight()
{
	return s_height;
}
void GameWindow::setResizedFlag()
{
	s_resizedFlag = true;
}
bool GameWindow::getReiszedFlag()
{
	return s_resizedFlag;
}
void GameWindow::setWidth(int width)
{
	s_width = width;
}
void GameWindow::setHeight(int height)
{
	s_height = height;
}
void GameWindow::setTerminated(bool terminated)
{
	s_terminated = terminated;
}
bool GameWindow::getTerminated()
{
	return s_terminated;
}
}