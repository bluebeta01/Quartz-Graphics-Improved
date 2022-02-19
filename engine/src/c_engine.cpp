#include "pch.h"
#include "c_engine.h"
#include "window/c_gamewindow.h"
#include "input/c_input.h"
#include "asset/c_assetmanager.h"
#include "asset/assets/c_textasset.h"
#include "asset/assets/c_modelasset.h"
#include "asset/filesystem/c_filestream.h"
#include "render/c_renderer.h"
#include "c_threadpool.h"

namespace Quartz
{
Engine::Engine() : m_editorMode(false)
{

}

void Engine::initialize(bool editorMode, HWND windowHandle)
{
	m_editorMode = editorMode;
	initializeSystems(windowHandle);
	if (!editorMode)
		initGameLoop();
}

void Engine::initGameLoop()
{
	while (!GameWindow::getTerminated())
		gameLoopBody();

	Renderer::deinitialize();
	AssetManager::deinitialize();
}

void Engine::gameLoopBody()
{
	static std::shared_ptr<ModelAsset> shiba = std::dynamic_pointer_cast<ModelAsset>(AssetManager::getAsset(AssetType::MODEL, "models/shiba.qmf"));
	static std::shared_ptr<ModelAsset> train;

	static std::vector<std::shared_ptr<ModelAsset>> trainAssets;

	static Transform ballTransform;
	ballTransform.position.z = 150;

	static float delta = 0;

	static long long oldFrameTime = 0;

	{
		long long frameTime1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		delta = (float)(frameTime1 - oldFrameTime) / 1000.0f;
		oldFrameTime = frameTime1;
		static float avg = 0;
		avg = avg + ((delta - avg) / 10);

		if (delta > 1.0f / 90.0f)
			QZINFO("Frame drop: {0}ms, {1}fps", delta * 1000.0f, 1.0f / delta);

		updateSystems();

		if (GameWindow::getReiszedFlag())
		{
			Renderer::resize(GameWindow::getWidth(), GameWindow::getHeight());
		}

		if (Input::isMouseButtonDown(0))
		{
			ballTransform.rotateY(5 * delta);
		}

		if (Input::isKeyDownThisFrame('U'))
		{
			for (int i = 0; i < 1000; i++)
			{
				trainAssets.push_back(std::dynamic_pointer_cast<ModelAsset>(AssetManager::getAsset(AssetType::MODEL, "models/train.qmf")));
			}
		}


		Renderer::beginRender();
		static float rot = 0;
		rot += 1 * delta;

		for (int i = 0; i < trainAssets.size(); i++)
			Renderer::renderModel(trainAssets[i], ballTransform);

		Renderer::renderModel(shiba, ballTransform);
		Renderer::renderModel(train, ballTransform);
		Renderer::endRender();
		
	}
}

void Engine::updateSystems()
{
	Input::update();
	GameWindow::update();
	AssetManager::update();
	Renderer::update();
}

void Engine::initializeSystems(HWND windowHandle)
{
	Log::initialize();
	Input::initialize();
	GameWindow::initialize(windowHandle);
	AssetManager::initialize();
	Renderer::initialize();
}
}