#include "pch.h"
#include "editor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include <dx/dxdevice.h>
#include <dx/dxrender3d.h>
#include <window/window.h>
#include <dx/dxtexture2d.h>
#include "window/renderwindow.h"
#include "colorpick.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Quartz::Editor
{
Engine Editor::m_engine;
std::vector<std::shared_ptr<EditorWindow>> Editor::m_windows;
Entity boxParent;
Entity box;

std::shared_ptr<Framebuffer> colorpickBuffer;

std::shared_ptr<ViewportSwapchain> viewportSwapchain;

void Editor::initialize()
{
	m_engine.initialize(true);

	boxParent = World::createEntity("boxparent", Transform(glm::vec3(0, 0, -10)), World::getRootScene());
	
	box = World::createEntity("box", Transform(glm::vec3(0, 0, 0)), World::getRootScene());
	EntityInfoComponent boxComp = World::getEntityRegister().get<EntityInfoComponent>(box);
	boxComp.setParent(boxParent);
	World::addComponentToEntity<EntityInfoComponent>(box, boxComp);

	RenderableComponent renderComp = {};
	renderComp.model = std::static_pointer_cast<ModelAsset>(AssetManager::getAsset(AssetType::MODEL, "models/blender/debug_object_1.qmf"));
	World::addComponentToEntity<RenderableComponent>(box, renderComp);

	initializeImgui();

	m_windows.push_back(std::static_pointer_cast<EditorWindow>(std::make_shared<RenderWindow>()));
	m_windows.back()->initialize();

	m_windows.push_back(std::static_pointer_cast<EditorWindow>(std::make_shared<RenderWindow>()));
	m_windows.back()->setName("View 2");
	m_windows.back()->initialize();

	m_windows.push_back(std::static_pointer_cast<EditorWindow>(std::make_shared<RenderWindow>()));
	m_windows.back()->setName("View 3");
	m_windows.back()->initialize();

	m_windows.push_back(std::static_pointer_cast<EditorWindow>(std::make_shared<RenderWindow>()));
	m_windows.back()->setName("View 4");
	m_windows.back()->initialize();

	while (!GameWindow::getTerminated())
		gameLoop();

	Renderer::deinitialize();
	AssetManager::deinitialize();
}

void Editor::gameLoop()
{
	m_engine.updateSystems();
	ImGui_ImplWin32_WndProcHandler(*GameWindow::getHandle(), Window::message, Window::wParam, Window::lParam);

	if (GameWindow::getReiszedFlag())
	{
		Renderer::s_device->waitForIdle();
		Renderer::s_swapchain->resize(GameWindow::getWidth(), GameWindow::getHeight(), GameWindow::getHandle());
		Renderer::s_device->waitForIdle();
	}
	

	for (std::shared_ptr<EditorWindow>& window : m_windows)
	{
		window->update();
	}

	if (Input::isMouseButtonDownThisFrame(0))
	{
		std::optional<Entity> entity = ColorPick::getColorPickEntity(GameWindow::getWidth(), GameWindow::getHeight(),
			Input::getCursorX(), Input::getCursorY(), glm::mat4(1.0f),
			glm::perspectiveFov(glm::radians(70.0f), (float)GameWindow::getWidth(), (float)GameWindow::getHeight(), 0.01f, 1000.0f));

		if (entity)
		{
			QZINFO("{0}", World::getEntityRegister().get<EntityInfoComponent>(*entity).name);
		}
		else
			QZINFO("No hit");
	}

	Renderer::beginRender();
	Renderer::setViewport(0, 0, (float)GameWindow::getWidth(), (float)GameWindow::getHeight());
	Renderer::setScissor(0, 0, GameWindow::getWidth(), GameWindow::getHeight());
	Renderer::clear();
	renderWindows();
	Renderer::endRender();
	Renderer::present();
}


void Editor::renderWindows()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);


	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 360.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		EntityInfoComponent info = World::getEntityRegister().get<EntityInfoComponent>(box);
		info.transform.rotation = glm::identity<glm::quat>();
		info.transform.rotateY(glm::radians(f));
		World::addComponentToEntity<EntityInfoComponent>(box, info);
	}

	for (std::shared_ptr<EditorWindow>& window : m_windows)
	{
		window->render();
	}

	ImGui::Render();

	int imguiFontHandleIndex = ((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getNextIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE imguiFontCpuHandle = ((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getCPUHandleOfIndex(imguiFontHandleIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE imguiFontGpuHandle = ((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getGPUHandleOfIndex(imguiFontHandleIndex);
	//This is a custom function that recreates the SRV for the imgui font texture becuase it will be overwritten
	ImGui_ImplDX12_CreateFontSRVAtHandle(imguiFontCpuHandle, imguiFontGpuHandle);

	ID3D12DescriptorHeap* heaps[] = { ((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getHeap() };
	((DxRender3D*)Renderer::s_render3d.get())->getCommandList()->SetDescriptorHeaps(1, heaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), ((DxRender3D*)Renderer::s_render3d.get())->getCommandList());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(NULL, (void*)((DxRender3D*)Renderer::s_render3d.get())->getCommandList());
	}
}

void Editor::initializeImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	float scale = 1.5f;
	ImFontConfig cfg;
	cfg.SizePixels = 20;
	io.Fonts->AddFontDefault(&cfg);

	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	ImGui_ImplWin32_Init(*GameWindow::getHandle());
	ImGui_ImplDX12_Init((ID3D12Device*)Renderer::s_device->getNativeResource(), Renderer::s_swapchain->getFramecount(),
		DXGI_FORMAT_R8G8B8A8_UNORM, ((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getHeap(),
		((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getHeap()->GetCPUDescriptorHandleForHeapStart(),
		((DxDevice*)Renderer::s_device.get())->m_gpuCbvSrvUavHeap->getHeap()->GetGPUDescriptorHandleForHeapStart());

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.34f, 0.34f, 0.34f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.69f, 0.59f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.27f, 0.27f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.09f, 0.09f, 0.09f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.27f, 0.54f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.52f, 0.23f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.64f, 0.31f, 0.08f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.43f, 0.21f, 0.05f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.72f, 0.45f, 0.22f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.72f, 0.45f, 0.22f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.79f, 0.49f, 0.24f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.45f, 0.41f, 0.37f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.38f, 0.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.79f, 0.50f, 0.24f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.45f, 0.41f, 0.37f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.82f, 0.38f, 0.00f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.37f, 0.26f, 0.86f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.75f, 0.10f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.24f, 0.84f, 0.19f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.27f, 0.27f, 0.27f, 0.54f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.27f, 0.27f, 0.27f, 0.54f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.27f, 0.27f, 0.27f, 0.54f);
	colors[ImGuiCol_Tab] = ImVec4(0.45f, 0.41f, 0.37f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.82f, 0.38f, 0.00f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.72f, 0.45f, 0.22f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.45f, 0.41f, 0.37f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.72f, 0.45f, 0.22f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
}