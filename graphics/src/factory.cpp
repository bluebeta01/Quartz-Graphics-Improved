#include "texture2d.h"
#include "dx\dxtexture2d.h"
#include "dx\dxcbuffer.h"
#include "dx\dxdevice.h"
#include "dx\dxframebuffer.h"
#include "dx\dxpipeline.h"
#include "dx\dxrender3d.h"
#include "dx\dxswapchain.h"
#include "dx\dxvbuffer.h"

std::shared_ptr<Texture2D> Texture2D::create(const Texture2DCreateInfo& createInfo)
{
	return std::make_shared<DxTexture2D>(createInfo);
}

std::shared_ptr<CBuffer> CBuffer::create(const CBufferCreateInfo& createInfo)
{
	return std::make_shared<DxCBuffer>(createInfo);
}

std::shared_ptr<Device> Device::create()
{
	return std::make_shared<DxDevice>();
}

void Framebuffer::releaseTextures()
{
	for (std::shared_ptr<Texture2D> texture : m_textures)
		texture->release();
}

std::shared_ptr<Framebuffer> Framebuffer::create(const FramebufferCreateInfo& createInfo)
{
	return std::make_shared<DxFramebuffer>(createInfo);
}

std::shared_ptr<Pipeline> Pipeline::create(const PipelineCreateInfo& createInfo)
{
	return std::make_shared<DxPipeline>(createInfo);
}

std::shared_ptr<Render3D> Render3D::create(std::shared_ptr<Device> device)
{
	return std::make_shared<DxRender3D>(device);
}

std::shared_ptr<Swapchain> Swapchain::create(const SwapchainCreateInfo& createInfo)
{
	return std::make_shared<DxSwapchain>(createInfo);
}

std::shared_ptr<VBuffer> VBuffer::create(const VBufferCreateInfo& createInfo)
{
	return std::make_shared<DxVBuffer>(createInfo);
}