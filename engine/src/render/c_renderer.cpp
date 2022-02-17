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
	s_threadPool.initialize((int)roundf((std::thread::hardware_concurrency() - 1) * RENDER_THREADS));
	QZINFO("Renderer thread count: {0}", s_threadPool.m_threadCount);

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

void Renderer::uploadAsset(std::shared_ptr<Asset> asset)
{
	
	switch (asset->m_type)
	{
	case Asset::Type::MODEL:
	{
		std::shared_ptr<ModelAsset> model = std::dynamic_pointer_cast<ModelAsset>(asset);
		model->m_vBuffer->bufferData(model->m_modelData.rawModelData->data(), sizeof(float) * model->m_modelData.rawModelData->size());
		break;
	}
	case Asset::Type::TEXTURE2D:
	{
		std::shared_ptr<TextureAsset> texture = std::dynamic_pointer_cast<TextureAsset>(asset);
		//s_render3D->scheduleTextureUpload(texture->m_texture2D);
		while (!Renderer::s_device->textureUploadReady());
		Renderer::s_device->uploadTexture(texture->m_texture2D, texture->m_textureData.rawTextureData->data(), texture->m_textureData.rawTextureData->size());
		break;
	}
	default:
		return;
	}
}

void Renderer::renderModel(std::shared_ptr<ModelAsset> model, const Transform& transform)
{
	if (!model || !model->childrenLoaded())
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
		model->m_material->m_shader->m_pipeline->findBindableDescriptionByName("matracies");

	std::shared_ptr<ShaderBindableDescription> textureBindableDesc =
		model->m_material->m_shader->m_pipeline->findBindableDescriptionByName("shaderTexture");

	std::shared_ptr<TextureAsset> diffuse = std::static_pointer_cast<TextureAsset>(model->m_material->m_material->getProperty("diffuse").m_value);

	s_render3d->bindPipeline(model->m_material->m_shader->m_pipeline);
	s_render3d->bindCBuffer(s_testBuffer, cBufferBindableDesc->tableIndex);
	if(textureBindableDesc != nullptr)
		s_render3d->bindTexture(diffuse->m_texture2D, textureBindableDesc->tableIndex);
	s_render3d->bindResources();
	s_render3d->renderVBuffer(model->m_vBuffer);
	//memcpy_s(s_cbuffer->getAddress(), sizeof(glm::mat4) * 4, mvp, sizeof(glm::mat4) * 4);

	//std::shared_ptr<TextureAsset> texture = std::static_pointer_cast<TextureAsset>(model->m_material->m_properties["diffuse"].m_value);

	//s_render3D->renderVBuffer(*model->m_vBuffer, *model->m_material->m_material, *s_cbuffer);
}

void Renderer::beginRender()
{
	//s_render3D->beginFrame(s_swapChain);
	//s_render3D->clearFrame(*s_swapChain);
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