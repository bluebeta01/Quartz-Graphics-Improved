#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include "swapchain.h"

class DxSwapchain : public Swapchain
{
public:
	DxSwapchain(const SwapchainCreateInfo& createInfo);

	std::shared_ptr<Framebuffer> acquireNextFrame();
	void waitForFrame();
	void releaseFrame();
	void present();
	void resize(int width, int height, void* renderSurface);

	NativeResource getNativeResource() const { return (NativeResource)m_swapchain; }
	IDXGISwapChain3* getSwapchain() const { return m_swapchain; }

private:
	IDXGISwapChain3* m_swapchain = nullptr;
	void makeSwapchain(void* renderSurface);
};