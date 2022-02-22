#include "pch.h"
#include "window/window.h"
#include "input/c_input.h"
#include "window/c_gamewindow.h"

namespace Quartz
{
namespace Window
{
HWND windowHandle;
MSG msg;
WPARAM wParam;
LPARAM lParam;
UINT message;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window::message = message;
    Window::wParam = wParam;
    Window::lParam = lParam;

    switch (message)
    {

    case WM_KEYDOWN:
    {
        Input::registerKeyDown((char)wParam);
        break;
    }
    case WM_KEYUP:
    {
        Input::registerKeyUp((char)wParam);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        Input::registerMouseButtonDown(0);
        break;
    }
    case WM_LBUTTONUP:
    {
        Input::registerMouseButtonUp(0);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        Input::registerMouseButtonDown(1);
        break;
    }
    case WM_RBUTTONUP:
    {
        Input::registerMouseButtonUp(1);
        break;
    }
    case WM_SIZE:
    {
        GameWindow::setResizedFlag();
        GameWindow::setWidth(LOWORD(lParam));
        GameWindow::setHeight(HIWORD(lParam));
        break;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void initialize(int width, int height, HWND hwnd)
{
    if (hwnd != nullptr)
    {
        windowHandle = hwnd;
        return;
    }

    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"WindowClass1";

    // register the window class
    RegisterClassEx(&wc);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window and use the result as the handle
    windowHandle = CreateWindowEx(NULL,
        L"WindowClass1",    // name of the window class
        L"Quartz Engine",   // title of the window
        WS_OVERLAPPEDWINDOW,    // window style
        100,    // x-position of the window
        100,    // y-position of the window
        rect.right - rect.left,    // width of the window
        rect.bottom - rect.top,    // height of the window
        NULL,    // we have no parent window, NULL
        NULL,    // we aren't using menus, NULL
        NULL,    // application handle
        NULL);    // used with multiple windows, NULL

    // display the window on the screen
    ShowWindow(windowHandle, 1);

    // enter the main loop:


}

void update()
{
    // Check to see if any messages are waiting in the queue
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            GameWindow::setTerminated(true);

        // translate keystroke messages into the right format
        TranslateMessage(&msg);

        // send the message to the WindowProc function
        DispatchMessage(&msg);
    }
}
}
}