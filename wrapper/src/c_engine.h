#pragma once
#include <engine.h>

namespace QzWrapper
{
public ref class Engine
{
public:
	Engine();
	~Engine();
	void Initialize(System::Boolean editorMode, System::IntPtr hwnd);
	void Update();

private:
	Quartz::Engine* nativePtr;
};
}