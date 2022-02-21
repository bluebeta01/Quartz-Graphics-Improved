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

void Renderer::renderWorld()
{
	auto view = World::getEntityRegister().view<const RenderableComponent, const EntityInfoComponent>();
	for (Entity entity : view)
	{
		renderEntity(entity);
	}
}

void Renderer::renderEntity(Entity entity)
{
	auto& entityInfo = World::getEntityRegister().get<const EntityInfoComponent>(entity);
	auto& entityRenderInfo = World::getEntityRegister().get<const RenderableComponent>(entity);

	CBuffer* cbuffer = entityRenderInfo.cbuffers[s_swapchain->getFrameIndex()].get();

	glm::mat4 projectionMatrix = glm::perspectiveFovRH(70.0f, (float)GameWindow::getWidth(), (float)GameWindow::getHeight(), 0.1f, 1000.0f);
	glm::mat4 viewMatrix = glm::identity<glm::mat4>();
	glm::mat4 modelMatrix = Transform::matrixFromTransform(entityInfo.transform, false);
	projectionMatrix = glm::transpose(projectionMatrix);
	modelMatrix = glm::transpose(modelMatrix);
	viewMatrix = glm::transpose(viewMatrix);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

	glm::mat4 mvp[] = { modelMatrix, viewMatrix, projectionMatrix, normalMatrix };
	cbuffer->bufferData(mvp, sizeof(float) * 57);

	if (!entityRenderInfo.model || !entityRenderInfo.model->dependenciesLoaded())
		return;

	s_render3d->bindPipeline(entityRenderInfo.model->getMaterial()->getShader()->getPipeline());

	for (std::shared_ptr<ShaderBindableDescription> bindable : entityRenderInfo.model->getMaterial()->getShader()->getPipeline()->getShaderBindableDescriptions())
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
				s_render3d->bindCBuffer(entityRenderInfo.cbuffers[s_swapchain->getFrameIndex()], bindable->tableIndex);
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
	s_render3d->setViewport(0, 0, GameWindow::getWidth(), GameWindow::getHeight());
	s_render3d->setScissorRect(0, 0, GameWindow::getWidth(), GameWindow::getHeight());
	clear();
}

void Renderer::endRender()
{
	s_render3d->endFrame();
	s_swapchain->present();

	//We can remove this when we impliment the correct use of cbuffers with our entities. Wiring to a cbuffer
	//while the shader is reading from it seems to be causing the driver to crash.
	//s_device->waitForIdle();
}
void Renderer::clear()
{
	s_render3d->clearFrame();
}
void Renderer::present()
{
	
}
void Renderer::resize(int width, int height)
{
	//s_swapChain->resize(width, height, GameWindow::getHandle());
	//s_render3D->resize(width, height);
}
}