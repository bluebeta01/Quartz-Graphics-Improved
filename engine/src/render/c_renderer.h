#pragma once
#include <memory>
#include "common.h"
#include "asset/assets/c_asset.h"
#include "asset/assets/c_modelasset.h"
#include "transform.h"
#include "c_threadpool.h"
#include "entity/entity.h"
#include <graphics.h>
#include "render/cbufferpool.h"

namespace Quartz
{
class Renderer
{
public:
	static std::shared_ptr<Device> s_device;
	static std::shared_ptr<Swapchain> s_swapchain;
	static std::shared_ptr<Render3D> s_render3d;
	static CBufferPool s_matrixBufferPool;

	static void initialize();
	static void deinitialize();
	static void update();
	static bool uploadAsset(std::shared_ptr<Asset> asset);
	static void renderWorld(const glm::mat4& viewMatrix);
	static void renderEntity(Entity entity, const glm::mat4& viewMatrix);
	static void beginRender();
	static void endRender();
	static void resize(int width, int height);
	static void setViewport(float x, float y, float width, float height);
	static void setScissor(int x, int y, int width, int height);
	static void clear();
	static void present();
	static void setOverridePipeline(std::shared_ptr<Pipeline> pipeline);

private:
	static ThreadPool s_threadPool;
	static std::shared_ptr<Pipeline> m_overridePipeline;
	Renderer();
};
}