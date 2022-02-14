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
GraphicsEngine Renderer::s_gengine;
std::shared_ptr<SwapChain> Renderer::s_swapChain;
std::shared_ptr<Render3D> Renderer::s_render3D;
std::shared_ptr<CBuffer> Renderer::s_cbuffer;

Renderer::Renderer()
{

}

void Renderer::initialize()
{
	s_threadPool.initialize((int)roundf((std::thread::hardware_concurrency() - 1) * RENDER_THREADS));
	QZINFO("Renderer thread count: {0}", s_threadPool.m_threadCount);
	
	s_gengine.initialize();
	s_swapChain = s_gengine.createSwapChain(GameWindow::getWidth(), GameWindow::getHeight(), GameWindow::getHandle());
	s_render3D = s_gengine.createRender3D(GameWindow::getWidth(), GameWindow::getHeight());
	s_cbuffer = s_gengine.createCBuffer(256);
}

void Renderer::deinitialize()
{
	s_threadPool.deinitialize();
	s_gengine.deinitialize();
}

void Renderer::update()
{
	s_render3D->uploadScheduledTextures();
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
		s_render3D->scheduleTextureUpload(texture->m_texture2D);
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
	memcpy_s(s_cbuffer->getAddress(), sizeof(glm::mat4) * 4, mvp, sizeof(glm::mat4) * 4);

	//std::shared_ptr<TextureAsset> texture = std::static_pointer_cast<TextureAsset>(model->m_material->m_properties["diffuse"].m_value);

	s_render3D->renderVBuffer(*model->m_vBuffer, *model->m_material->m_material, *s_cbuffer);
}

void Renderer::beginRender()
{
	s_render3D->beginFrame(s_swapChain);
	s_render3D->clearFrame(*s_swapChain);
}

void Renderer::endRender()
{
	s_render3D->endFrame(*s_swapChain);
}
void Renderer::clear()
{
	float color[] = { 0.7f, 0.7f, 0.7f, 1.0f };
}
void Renderer::present()
{
	
}
void Renderer::resize(int width, int height)
{
	s_swapChain->resize(width, height, GameWindow::getHandle());
	s_render3D->resize(width, height);
}
}