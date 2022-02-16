#pragma once
#include <memory>
#include <vector>
#include "device.h"
#include "framebuffer.h"

struct SwapchainCreateInfo
{
	std::shared_ptr<Device> device;
	int width;
	int height;
	int textureArraySize;
	int frameCount;
	void* renderSurface;
};

class Swapchain : Resource
{
public:
	Swapchain(const SwapchainCreateInfo& createInfo) :
		m_device(createInfo.device), m_width(createInfo.width), m_height(createInfo.height),
		m_textureArraySize(createInfo.textureArraySize), m_frameCount(createInfo.frameCount)
	{}

	static std::shared_ptr<Swapchain> create(const SwapchainCreateInfo& createInfo);
	virtual std::shared_ptr<Framebuffer> acquireNextFrame() = 0;
	virtual void waitForFrame() = 0;
	virtual void releaseFrame() = 0;
	virtual void present() = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	int getTextureArraySize() const { return m_textureArraySize; }
	int getFramecount() const { return m_frameCount; }
	int getFrameIndex() const { return m_frameIndex; }
	const std::vector<std::shared_ptr<Framebuffer>>& getFramebuffers() const { return m_framebuffers; }

protected:
	std::shared_ptr<Device> m_device;
	int m_width = 0;
	int m_height = 0;
	int m_textureArraySize = 0;
	int m_frameCount = 0;
	int m_frameIndex = 0;
	std::vector<std::shared_ptr<Framebuffer>> m_framebuffers;
};