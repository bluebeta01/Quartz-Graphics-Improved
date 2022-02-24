#include "pch.h"
#include "renderwindow.h"
#include "imgui/imgui.h"
#include <editor.h>
#include <d3d12.h>

namespace Quartz::Editor
{
void RenderWindow::initialize()
{
	SwapchainCreateInfo info = {};
	info.device = Renderer::s_device;
	info.frameCount = Renderer::s_swapchain->getFramecount();
	info.height = 1080;
	info.width = 1920;
	info.textureArraySize = 1;
	m_viewportSwapchain = std::static_pointer_cast<ViewportSwapchain>(ViewportSwapchain::create(info));

	m_wireframeShader = std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(AssetType::SHADER, "shaders/wireframe.qshader"));
}
void RenderWindow::update()
{
	if (m_resized)
	{
		Renderer::s_device->waitForIdle();
		m_viewportSwapchain->resize(m_width, m_height, nullptr);
	}

	m_viewportFramebuffer = m_viewportSwapchain->acquireNextFrame();
	m_viewportSwapchain->waitForFrame();

	Renderer::s_render3d->beginFrame(m_viewportFramebuffer);
	Renderer::s_render3d->setViewport(0, 0, m_viewportWidth, m_viewportHeight);
	Renderer::s_render3d->setScissorRect(0, 0, m_viewportWidth, m_viewportHeight);
	Renderer::s_render3d->clearFrame();
	if(m_wireframeEnabled)
		Renderer::setOverridePipeline(m_wireframeShader->getPipeline());
	Renderer::renderWorld(glm::mat4(1.0f));
	Renderer::s_render3d->endFrame();
	Renderer::setOverridePipeline(nullptr);
}
void RenderWindow::render()
{
	Renderer::s_render3d->transitionViewportTextureForPresent(m_viewportFramebuffer->getTextures()[0]);
	std::shared_ptr<D3D12_GPU_DESCRIPTOR_HANDLE> viewportGpuHandle = std::static_pointer_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(Renderer::s_render3d->uploadViewportDescriptor(m_viewportFramebuffer->getTextures()[0]));

	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		if (ImGui::Begin(m_name.c_str(), &m_visible, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse))
		{
			ImVec2 currentWindowSize = ImGui::GetWindowSize();
			if (m_width != currentWindowSize.x || m_height != currentWindowSize.y)
				m_resized = true;
			else
				m_resized = false;
			m_width = currentWindowSize.x;
			m_height = currentWindowSize.y;

			ImGui::PopStyleVar();
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Lighting"))
				{
					ImGui::MenuItem("Combined", NULL, false);
					ImGui::MenuItem("Realtime", NULL, false);
					ImGui::MenuItem("Lightmap", NULL, false);
					ImGui::MenuItem("Unlit", NULL, false);
					ImGui::MenuItem("Wireframe", NULL, &m_wireframeEnabled);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

			ImTextureID texid = (void*)viewportGpuHandle->ptr;
			
			ImVec2 winSize = ImGui::GetWindowSize();
			winSize.y -= ImGui::GetFrameHeight() * 2;
			m_viewportWidth = winSize.x;
			m_viewportHeight = winSize.y;
			ImGui::Image(texid, winSize);
			
		}
			ImGui::End();
			ImGui::PopStyleVar();

	}

	Renderer::s_render3d->transitionViewportTextureForRenderPas(m_viewportFramebuffer->getTextures()[0]);
}
void RenderWindow::renderViewport()
{
	
}
}