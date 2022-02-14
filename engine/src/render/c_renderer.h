#pragma once
#include <memory>
#include <graphicsengine.h>
#include "common.h"
#include "asset/assets/c_asset.h"
#include "asset/assets/c_modelasset.h"
#include "transform.h"
#include "c_threadpool.h"

namespace Quartz
{
class Renderer
{
public:
	static void initialize();
	static void deinitialize();
	static void update();
	static void uploadAsset(std::shared_ptr<Asset> asset);
	static void renderModel(std::shared_ptr<ModelAsset> asset, const Transform& transform);
	static void beginRender();
	static void endRender();
	static void resize(int width, int height);

	static GraphicsEngine s_gengine;
	static std::shared_ptr<SwapChain> s_swapChain;
	static std::shared_ptr<Render3D> s_render3D;
	static std::shared_ptr<CBuffer> s_cbuffer;

private:
	static ThreadPool s_threadPool;
	static void clear();
	static void present();
	Renderer();
};
}