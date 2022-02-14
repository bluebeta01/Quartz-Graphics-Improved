#include "c_engine.h"

namespace QzWrapper
{
Engine::Engine() : nativePtr(new Quartz::Engine())
{

}

Engine::~Engine()
{
	delete nativePtr;
}

void Engine::Initialize(System::Boolean editorMode, System::IntPtr hwnd)
{
	nativePtr->initialize(editorMode, (HWND)hwnd.ToPointer());
}

void Engine::Update()
{
	nativePtr->gameLoopBody();
}
}