#include "pch.h"
#include "colorpick.h"
#include <unordered_map>

namespace Quartz::Editor
{
std::optional<Entity> Quartz::Editor::ColorPick::getColorPickEntity(int viewportWidth, int viewportHeight, int pickX, int pickY,
	const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	Texture2DCreateInfo texInfo = {};
	texInfo.arraySize = 1;
	texInfo.device = Renderer::s_device;
	texInfo.height = GameWindow::getHeight();
	texInfo.width = GameWindow::getWidth();
	texInfo.type = TextureType::COLORPICK;
	texInfo.textureResource = nullptr;
	std::shared_ptr<Texture2D> renderTexture = Texture2D::create(texInfo);

	texInfo.type = TextureType::DEPTH_STENCIL;
	std::shared_ptr<Texture2D> dsTexture = Texture2D::create(texInfo);

	std::vector<std::shared_ptr<Texture2D>> textures = { renderTexture, dsTexture };

	FramebufferCreateInfo fbInfo = {};
	fbInfo.device = Renderer::s_device;
	fbInfo.textureCount = 1;
	fbInfo.textures = &textures;
	std::shared_ptr<Framebuffer> colorpickBuffer = Framebuffer::create(fbInfo);

	Renderer::s_render3d->beginFrame(colorpickBuffer);
	Renderer::s_render3d->setViewport(0, 0, viewportWidth, viewportHeight);
	Renderer::s_render3d->setScissorRect(0, 0, viewportWidth, viewportHeight);
	Renderer::s_render3d->clearFrame();

	uint32_t nextColorValue = 1;
	std::unordered_map<uint32_t, Entity> entityMap;

	auto view = World::getEntityRegister().view<const RenderableComponent, const EntityInfoComponent>();
	for (Entity entity : view)
	{
		uint32_t colors[3] = { nextColorValue, 0, 0 };
		entityMap[nextColorValue] = entity;
		nextColorValue++;
		Renderer::renderWorldColorpick(glm::mat4(1.0f), glm::perspectiveFov(glm::radians(70.0f), (float)viewportWidth, (float)viewportHeight, 0.01f, 1000.0f), colors);
	}
	Renderer::s_render3d->endFrame();
	Renderer::s_device->waitForIdle();

	std::vector<uint8_t> data = Renderer::s_device->readbackTexture(colorpickBuffer->getTextures()[0]);

	struct Pixel
	{
		unsigned int rgba[4];
	};

	std::vector<Pixel> pixelData(data.size() / 16);
	memcpy_s(pixelData.data(), sizeof(Pixel) * pixelData.size(), data.data(), data.size());

	int index = pickX + (pickY * viewportWidth);
	Pixel& p = pixelData[index];

	colorpickBuffer->releaseTextures();

	auto& e = entityMap.find(p.rgba[0]);
	if (e != entityMap.end())
		return (*e).second;
	return std::nullopt;
}
}