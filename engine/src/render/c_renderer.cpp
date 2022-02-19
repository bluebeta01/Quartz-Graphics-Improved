#include "pch.h"
#include "render/c_renderer.h"
#include "window/c_gamewindow.h"
#include "asset/c_assetmanager.h"
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
std::shared_ptr<CBuffer> Renderer::s_testBuffer;

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

	CBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.device = s_device;
	bufferCreateInfo.size = sizeof(float) * 57;
	s_testBuffer = CBuffer::create(bufferCreateInfo);
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

void Renderer::renderModel(std::shared_ptr<ModelAsset> model, const Transform& transform)
{
	if (!model)
		return;

	if (!model->m_loadedCacheValid)
		if (model->dependenciesLoaded())
		{
			model->m_loadedCache = true;
			model->m_loadedCacheValid = true;
		}
		else
			return;
	else
		if (!model->m_loadedCache)
			return;
	

	glm::mat4 projectionMatrix = glm::perspectiveFovLH(70.0f, (float)GameWindow::getWidth(), (float)GameWindow::getHeight(), 0.1f, 1000.0f);
	glm::mat4 viewMatrix = glm::identity<glm::mat4>();
	glm::mat4 modelMatrix = Transform::matrixFromTransform(transform, false);
	projectionMatrix = glm::transpose(projectionMatrix);
	modelMatrix = glm::transpose(modelMatrix);
	viewMatrix = glm::transpose(viewMatrix);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));

	glm::mat4 mvp[] = { modelMatrix, viewMatrix, projectionMatrix, normalMatrix };
	s_testBuffer->bufferData(mvp, sizeof(float) * 57);

	std::shared_ptr<ShaderBindableDescription> cBufferBindableDesc =
		model->getMaterial()->getShader()->getPipeline()->findBindableDescriptionByName("matracies");

	std::shared_ptr<ShaderBindableDescription> textureBindableDesc =
		model->getMaterial()->getShader()->getPipeline()->findBindableDescriptionByName("shaderTexture");

	std::shared_ptr<TextureAsset> diffuse = std::static_pointer_cast<TextureAsset>(model->getMaterial()->getProperty("diffuse").m_value);

	s_render3d->bindPipeline(model->getMaterial()->getShader()->getPipeline());
	if(cBufferBindableDesc != nullptr)
		s_render3d->bindCBuffer(s_testBuffer, cBufferBindableDesc->tableIndex);
	if(textureBindableDesc != nullptr)
		s_render3d->bindTexture(diffuse->getTexture(), textureBindableDesc->tableIndex);
	s_render3d->bindResources();
	s_render3d->renderVBuffer(model->getVBuffer());
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
	s_device->waitForIdle();
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