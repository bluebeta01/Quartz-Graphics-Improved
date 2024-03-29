#include "viewportswapchain.h"

ViewportSwapchain::ViewportSwapchain(const SwapchainCreateInfo& createInfo) :
	Swapchain(createInfo)
{
	makeSwapchain();
}

std::shared_ptr<Swapchain> ViewportSwapchain::create(const SwapchainCreateInfo& createInfo)
{
	return std::make_shared<ViewportSwapchain>(createInfo);
}

std::shared_ptr<Framebuffer> ViewportSwapchain::acquireNextFrame()
{
	m_frameIndex = (m_frameIndex + 1) % m_frameCount;
	return m_framebuffers[m_frameIndex];
}

void ViewportSwapchain::waitForFrame()
{
	m_framebuffers[m_frameIndex]->waitIfNotReady();
}

void ViewportSwapchain::releaseFrame()
{
}

void ViewportSwapchain::present()
{
}

std::shared_ptr<Framebuffer> ViewportSwapchain::getFrontBuffer()
{
	for (int i = 0; i < m_frameCount; i++)
	{
		if (m_framebuffers[(m_frameIndex - i) % m_frameCount]->readyForRender())
			return m_framebuffers[(m_frameIndex - i) % m_frameCount];
	}
	return m_framebuffers[m_frameIndex];
}

void ViewportSwapchain::resize(int width, int height, void* renderSurface)
{
	m_width = width;
	m_height = height;
	for (std::shared_ptr<Framebuffer> framebuffer : m_framebuffers)
		framebuffer->releaseTextures();
	m_framebuffers.clear();

	makeSwapchain();
}

void ViewportSwapchain::makeSwapchain()
{
	for (int i = 0; i < m_frameCount; i++)
	{
		Texture2DCreateInfo texInfo = {};
		texInfo.arraySize = 1;
		texInfo.device = m_device;
		texInfo.height = m_height;
		texInfo.width = m_width;
		texInfo.type = TextureType::RENDER_TARGET;
		texInfo.textureResource = nullptr;
		texInfo.createSrvForRenderTarget = true;
		std::shared_ptr<Texture2D> renderTexture = Texture2D::create(texInfo);

		texInfo.type = TextureType::DEPTH_STENCIL;
		std::shared_ptr<Texture2D> dsTexture = Texture2D::create(texInfo);

		std::vector<std::shared_ptr<Texture2D>> textures = { renderTexture, dsTexture };

		FramebufferCreateInfo fbInfo = {};
		fbInfo.device = m_device;
		fbInfo.textureCount = 2;
		fbInfo.textures = &textures;
		m_framebuffers.push_back(Framebuffer::create(fbInfo));
	}
}
