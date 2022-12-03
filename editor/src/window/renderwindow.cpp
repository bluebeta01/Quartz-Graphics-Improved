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
		m_viewportSwapchain->resize(m_viewportWidth, m_viewportHeight, nullptr);
	}

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);

	static float orthoScale = 0.01f;
	if (Input::isMouseButtonDown(0))
		orthoScale -= 0.0001f;
	if (Input::isMouseButtonDown(1))
		orthoScale += 0.0001f;

	if (m_viewSelection[View::PERSPECTIVE])
		projectionMatrix = glm::perspectiveFov(glm::radians(70.0f), m_viewportWidth, m_viewportHeight, 0.01f, 1000.0f);
	if (m_viewSelection[View::FRONT])
		projectionMatrix = glm::ortho(-1.0f * orthoScale * m_viewportWidth, 1.0f * orthoScale * m_viewportWidth, -1.0f * orthoScale * m_viewportHeight, 1.0f * orthoScale * m_viewportHeight, -1000.0f, 1000.0f);

	m_viewportFramebuffer = m_viewportSwapchain->acquireNextFrame();
	m_viewportSwapchain->waitForFrame();

	Renderer::s_render3d->beginFrame(m_viewportFramebuffer);
	Renderer::s_render3d->setViewport(0, 0, m_viewportWidth, m_viewportHeight);
	Renderer::s_render3d->setScissorRect(0, 0, m_viewportWidth, m_viewportHeight);
	Renderer::s_render3d->clearFrame();
	if(m_lightingSelection[Lighting::WIREFRAME])
		Renderer::setOverridePipeline(m_wireframeShader->getPipeline());
	Renderer::renderWorld(viewMatrix, projectionMatrix);
	Renderer::s_render3d->endFrame();
	Renderer::setOverridePipeline(nullptr);
}
void RenderWindow::render()
{
	Renderer::s_render3d->transitionViewportTextureForPresent(m_viewportFramebuffer->getTextures()[0]);
	std::shared_ptr<D3D12_GPU_DESCRIPTOR_HANDLE> viewportGpuHandle = std::static_pointer_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(Renderer::s_render3d->uploadViewportDescriptor(m_viewportFramebuffer->getTextures()[0]));

	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
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
					if (ImGui::MenuItem("Combined", NULL, m_lightingSelection[Lighting::COMBINED]))
						setLighting(Lighting::COMBINED);
					if (ImGui::MenuItem("Realtime", NULL, m_lightingSelection[Lighting::REALTIME]))
						setLighting(Lighting::REALTIME);
					if (ImGui::MenuItem("Lightmap", NULL, m_lightingSelection[Lighting::LIGHTMAP]))
						setLighting(Lighting::LIGHTMAP);
					if (ImGui::MenuItem("Unlit", NULL, m_lightingSelection[Lighting::UNLIT]))
						setLighting(Lighting::UNLIT);
					if (ImGui::MenuItem("Wireframe", NULL, m_lightingSelection[Lighting::WIREFRAME]))
						setLighting(Lighting::WIREFRAME);
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("View"))
				{
					if (ImGui::MenuItem("Perspective", NULL, m_viewSelection[View::PERSPECTIVE]))
						setViewType(View::PERSPECTIVE);
					if (ImGui::MenuItem("Front", NULL, m_viewSelection[View::FRONT]))
						setViewType(View::FRONT);
					if (ImGui::MenuItem("Top", NULL, m_viewSelection[View::TOP]))
						setViewType(View::TOP);
					if (ImGui::MenuItem("Left", NULL, m_viewSelection[View::LEFT]))
						setViewType(View::LEFT);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

			ImTextureID texid = (void*)viewportGpuHandle->ptr;
			
			ImVec2 winSize = ImGui::GetWindowSize();
			winSize.y -= ImGui::GetFrameHeight() * 2;
			m_viewportWidth = std::fmax(winSize.x, 10.0f);
			m_viewportHeight = std::fmax(winSize.y, 10.0f);
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
void RenderWindow::setLighting(Lighting lighting)
{
	for (int i = 0; i < m_lightingSelection.size(); i++)
	{
		m_lightingSelection[i] = false;
	}
	m_lightingSelection[lighting] = true;
}
void RenderWindow::setViewType(View view)
{
	for (int i = 0; i < m_viewSelection.size(); i++)
	{
		m_viewSelection[i] = false;
	}
	m_viewSelection[view] = true;
}
}