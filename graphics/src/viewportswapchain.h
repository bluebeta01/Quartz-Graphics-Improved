#pragma once
#include "swapchain.h"

class ViewportSwapchain : public Swapchain
{
public:
	ViewportSwapchain(const SwapchainCreateInfo& createInfo);

	static std::shared_ptr<Swapchain> create(const SwapchainCreateInfo& createInfo);
	NativeResource getNativeResource() const { return nullptr; }
	std::shared_ptr<Framebuffer> acquireNextFrame();
	void waitForFrame();
	void releaseFrame();
	void present();
	std::shared_ptr<Framebuffer> getFrontBuffer();

private:

};