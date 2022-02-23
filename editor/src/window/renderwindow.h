#pragma once
#include "editorwindow.h"
#include <graphics.h>

namespace Quartz::Editor
{
class RenderWindow : public EditorWindow
{
public:
	void initialize();
	void update();
	void render();

private:
	std::shared_ptr<ViewportSwapchain> m_viewportSwapchain;
	std::shared_ptr<Framebuffer> m_viewportFramebuffer;
	std::shared_ptr<ShaderAsset> m_wireframeShader;
	bool m_wireframeEnabled = false;
	float m_viewportWidth = 0;
	float m_viewportHeight = 0;
	void renderViewport();
};
}