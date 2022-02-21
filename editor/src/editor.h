#pragma once
#include <engine.h>
#include <memory>

namespace Quartz::Editor
{
class Editor
{
public:
	static void initialize();
	static Quartz::Engine m_engine;

private:
	static void gameLoop();
};
}