#pragma once
#include <memory>
#include "common.h"
#include "asset/assets/c_asset.h"
#include "asset/assets/c_modelasset.h"
#include "transform.h"
#include "c_threadpool.h"
#include <graphics.h>

namespace Quartz
{
class Renderer
{
public:
	static std::shared_ptr<Device> s_device;
	static std::shared_ptr<Swapchain> s_swapchain;
	static std::shared_ptr<Render3D> s_render3d;

	static void initialize();
	static void deinitialize();
	static void update();
	static void uploadAsset(std::shared_ptr<Asset> asset);
	static void renderModel(std::shared_ptr<ModelAsset> asset, const Transform& transform);
	static void beginRender();
	static void endRender();
	static void resize(int width, int height);

private:
	static ThreadPool s_threadPool;
	static void clear();
	static void present();
	Renderer();
};
}