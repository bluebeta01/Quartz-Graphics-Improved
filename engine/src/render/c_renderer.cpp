#include "pch.h"
#include "render/c_renderer.h"
#include "window/c_gamewindow.h"
#include "asset/c_assetmanager.h"
#include "entity\world.h"
#include "entity/component/entityinfo.h"
#include "entity/component/renderable.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

namespace Quartz
{
ThreadPool Renderer::s_threadPool;
std::shared_ptr<Device> Renderer::s_device;
std::shared_ptr<Swapchain> Renderer::s_swapchain;
std::shared_ptr<Render3D> Renderer::s_render3d;
CBufferPool Renderer::s_matrixBufferPool(100, sizeof(float) * 57);
std::shared_ptr<Pipeline> Renderer::m_overridePipeline;

Renderer::Renderer()
{

}

void Renderer::initialize()
{
	//s_threadPool.initialize((int)roundf((std::thread::hardware_concurrency() - 1) * RENDER_THREADS));
	//QZINFO("Renderer thread count: {0}", s_threadPool.m_threadCount);

	s_device = Device::create();
	
	SwapchainCreateInfo info = {};
	info.device = s_device;
	info.frameCount = 3;
	info.height = 720;
	info.width = 1280;
	info.textureArraySize = 1;
	info.renderSurface = GameWindow::getHandle();

	s_swapchain = Swapchain::create(info);

	s_render3d = Render3D::create(s_device);

	s_matrixBufferPool.initialize();
}

void Renderer::deinitialize()
{
	s_threadPool.deinitialize();
}

void Renderer::update()
{
	
}

bool Renderer::uploadAsset(std::shared_ptr<Asset> asset)
{
	switch (asset->getType())
	{
	case AssetType::MODEL:
	{
		std::shared_ptr<ModelAsset> model = std::dynamic_pointer_cast<ModelAsset>(asset);
		model->getVBuffer()->bufferData(model->getRawData()->data(), model->getRawData()->size() * sizeof(float));
		break;
	}
	case AssetType::TEXTURE2D:
	{
		std::shared_ptr<TextureAsset> texture = std::dynamic_pointer_cast<TextureAsset>(asset);
		if (Renderer::s_device->textureUploadReady())
			Renderer::s_device->uploadTexture(texture->getTexture(), texture->getRawData()->data(), texture->getRawData()->size());
		else
			return false;
		break;
	}
	default:
		return true;
	}
}

void Renderer::renderWorld(const glm::mat4& viewMatrix)
{
	auto view = World::getEntityRegister().view<const RenderableComponent, const EntityInfoComponent>();
	for (Entity entity : view)
	{
		renderEntity(entity, viewMatrix);
	}
}

void Renderer::renderEntity(Entity entity, const glm::mat4& viewMatrix)
{
	auto& entityInfo = World::getEntityRegister().get<const EntityInfoComponent>(entity);
	auto& entityRenderInfo = World::getEntityRegister().get<const RenderableComponent>(entity);

	if (!entityRenderInfo.model || !entityRenderInfo.model->dependenciesLoaded())
		return;

	std::shared_ptr<Pipeline> pipeline;

	if (!m_overridePipeline)
		pipeline = entityRenderInfo.model->getMaterial()->getShader()->getPipeline();
	else
		pipeline = m_overridePipeline;

	s_render3d->bindPipeline(pipeline);
	
	for (std::shared_ptr<ShaderBindableDescription> bindable : pipeline->getShaderBindableDescriptions())
	{
		switch (bindable->type)
		{
		case ShaderVariableType::TEXTURE:
		{
			const MaterialProperty* matProp = entityRenderInfo.model->getMaterial()->getProperty(bindable->name);
			if (!matProp)
				continue;
			if (matProp->m_type == MaterialProperty::Type::TEXTURE2D)
				s_render3d->bindTexture(std::static_pointer_cast<TextureAsset>(matProp->m_value)->getTexture(), bindable->tableIndex);
			break;
		}
		case ShaderVariableType::CBUFFER:
		{
			if (bindable->name == "matracies")
			{
				std::shared_ptr<CBuffer> buffer = s_matrixBufferPool.getNextBuffer();

				glm::mat4 projectionMatrix = glm::perspectiveFovRH(70.0f, (float)GameWindow::getWidth(), (float)GameWindow::getHeight(), 0.1f, 1000.0f);
				glm::mat4 viewMatrixTransposed = viewMatrix;
				glm::mat4 modelMatrix = Transform::matrixFromTransform(entityInfo.getWorldTransform(), false);
				projectionMatrix = glm::transpose(projectionMatrix);
				modelMatrix = glm::transpose(modelMatrix);
				viewMatrixTransposed = glm::transpose(viewMatrix);
				glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

				glm::mat4 mvp[] = { modelMatrix, viewMatrixTransposed, projectionMatrix, normalMatrix };
				buffer->bufferData(mvp, sizeof(float) * 57);

				s_render3d->bindCBuffer(buffer, bindable->tableIndex);
				//s_render3d->bindCBuffer(entityRenderInfo.cbuffers[s_swapchain->getFrameIndex()], bindable->tableIndex);
			}
			break;
		}
		}
	}

	s_render3d->bindResources();
	s_render3d->renderVBuffer(entityRenderInfo.model->getVBuffer());

}

void Renderer::beginRender()
{
	std::shared_ptr<Framebuffer> fb = s_swapchain->acquireNextFrame();
	s_swapchain->waitForFrame();
	s_render3d->beginFrame(fb);
}

void Renderer::endRender()
{
	s_render3d->endFrame();
}
void Renderer::clear()
{
	s_render3d->clearFrame();
}
void Renderer::present()
{
	s_swapchain->present();
}
void Renderer::setOverridePipeline(std::shared_ptr<Pipeline> pipeline)
{
	if (pipeline)
		m_overridePipeline = pipeline;
	else
		m_overridePipeline = nullptr;
}
void Renderer::resize(int width, int height)
{
	//s_swapChain->resize(width, height, GameWindow::getHandle());
	//s_render3D->resize(width, height);
}
void Renderer::setViewport(float x, float y, float width, float height)
{
	s_render3d->setViewport(x, y, width, height);
}
void Renderer::setScissor(int x, int y, int width, int height)
{
	s_render3d->setScissorRect(x, y, width, height);
}
}