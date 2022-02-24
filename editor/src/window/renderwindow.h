#pragma once
#include "editorwindow.h"
#include <graphics.h>
#include <array>

namespace Quartz::Editor
{
class RenderWindow : public EditorWindow
{
public:
	void initialize();
	void update();
	void render();
	
	enum Lighting { COMBINED = 0, REALTIME, LIGHTMAP, UNLIT, WIREFRAME };
	enum View { PERSPECTIVE = 0, FRONT, TOP, LEFT };

private:
	std::shared_ptr<ViewportSwapchain> m_viewportSwapchain;
	std::shared_ptr<Framebuffer> m_viewportFramebuffer;
	std::shared_ptr<ShaderAsset> m_wireframeShader;
	std::array<bool, 5> m_lightingSelection = {true};
	std::array<bool, 4> m_viewSelection = { true };
	float m_viewportWidth = 100;
	float m_viewportHeight = 100;
	void renderViewport();
	void setLighting(Lighting lighting);
	void setViewType(View view);
};
}