#pragma once
#include <engine.h>
#include <memory>
#include "window/editorwindow.h"

namespace Quartz::Editor
{
class Editor
{
public:
	static void initialize();
	static Quartz::Engine m_engine;

private:
	static std::vector<std::shared_ptr<EditorWindow>> m_windows;
	static void gameLoop();
	static void renderWindows();
};
}