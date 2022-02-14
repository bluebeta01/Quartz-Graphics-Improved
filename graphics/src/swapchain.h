#pragma once
#include <memory>
#include <vector>
#include "device.h"
#include "framebuffer.h"
#include "descheap.h"

struct SwapchainCreateInfo
{
	std::shared_ptr<Device> device;
	int width;
	int height;
	int textureArraySize;
	int frameCount;
	void* renderSurface;
	std::shared_ptr<DescriptorHeap> depthStencilViewHeap;
	std::shared_ptr<DescriptorHeap> renderTargetViewHeap;
};

class Swapchain : Resource
{
public:
	Swapchain(const SwapChainCreateInfo& createInfo) :
		m_device(createInfo.device), m_width(createInfo.width), m_height(createInfo.height),
		m_textureArraySize(createInfo.textureArraySize), m_frameCount(createInfo.frameCount),
		m_depthStencilViewHeap(createInfo.depthStencilViewHeap), m_renderTargetViewHeap(createInfo.renderTargetViewHeap)
	{}

	virtual std::shared_ptr<Framebuffer> acquireNextFrame() = 0;
	virtual void waitForFrame() = 0;
	virtual void releaseFrame() = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	int getTextureArraySize() const { return m_textureArraySize; }
	int getFramecount() const { return m_frameCount; }
	std::shared_ptr<DescriptorHeap> getDepthStencilViewHeap() const { return m_depthStencilViewHeap; }
	std::shared_ptr<DescriptorHeap> getRenderTargetViewHeap() const { return m_renderTargetViewHeap; }
	int getFrameIndex() const { return m_frameIndex; }
	const std::vector<std::shared_ptr<Framebuffer>>& m_framebuffers;

private:
	std::shared_ptr<Device> m_device;
	int m_width;
	int m_height;
	int m_textureArraySize;
	int m_frameCount;
	std::shared_ptr<DescriptorHeap> m_depthStencilViewHeap;
	std::shared_ptr<DescriptorHeap> m_renderTargetViewHeap;
	int m_frameIndex;
	std::vector<std::shared_ptr<Framebuffer>> m_framebuffers;
};