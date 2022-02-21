#include "dxswapchain.h"
#include "dxdevice.h"
#include "dxframebuffer.h"

DxSwapchain::DxSwapchain(const SwapchainCreateInfo& createInfo) :
	Swapchain(createInfo)
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = m_frameCount;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	std::shared_ptr<DxDevice> device = std::static_pointer_cast<DxDevice>(m_device);
	HWND* hwnd = (HWND*)createInfo.renderSurface;
	IDXGISwapChain1* swapChain;
	device->getFactory()->CreateSwapChainForHwnd(
		device->getQueue(),
		*hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);

	device->getFactory()->MakeWindowAssociation(*hwnd, DXGI_MWA_NO_ALT_ENTER);
	m_swapchain = (IDXGISwapChain3*)swapChain;
	m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();

	Texture2DCreateInfo dsTextureInfo = {};
	dsTextureInfo.arraySize = 1;
	dsTextureInfo.textureResource = nullptr;
	dsTextureInfo.device = m_device;
	dsTextureInfo.height = m_height;
	dsTextureInfo.width = m_width;
	dsTextureInfo.type = TextureType::DEPTH_STENCIL;

	Texture2DCreateInfo rtTextureInfo = {};
	rtTextureInfo.arraySize = 1;
	rtTextureInfo.textureResource = nullptr;
	rtTextureInfo.device = m_device;
	rtTextureInfo.height = m_height;
	rtTextureInfo.width = m_width;
	rtTextureInfo.type = TextureType::RENDER_TARGET;

	for (int frame = 0; frame < m_frameCount; frame++)
	{
		ID3D12Resource* rtTextureResource;
		m_swapchain->GetBuffer(frame, IID_PPV_ARGS(&rtTextureResource));
		rtTextureInfo.textureResource = (void*)rtTextureResource;
		std::shared_ptr<Texture2D> dsTexture = Texture2D::create(dsTextureInfo);
		std::shared_ptr<Texture2D> rtTexture = Texture2D::create(rtTextureInfo);

		std::vector<std::shared_ptr<Texture2D>> textures = { rtTexture, dsTexture };

		FramebufferCreateInfo fbInfo = {};
		fbInfo.device = m_device;
		fbInfo.textureCount = textures.size();
		fbInfo.textures = &textures;
		std::shared_ptr<Framebuffer> framebuffer = Framebuffer::create(fbInfo);
		m_framebuffers.push_back(framebuffer);
	}


	
}

std::shared_ptr<Framebuffer> DxSwapchain::acquireNextFrame()
{
	m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();
	return m_framebuffers[m_frameIndex];
}

void DxSwapchain::waitForFrame()
{
	std::shared_ptr<DxFramebuffer> framebuffer = std::static_pointer_cast<DxFramebuffer>(m_framebuffers[m_frameIndex]);
	if (framebuffer->getFence()->GetCompletedValue() <= framebuffer->getFenceValue())
	{
		HRESULT r = framebuffer->getFence()->SetEventOnCompletion(framebuffer->getFenceValue(), framebuffer->getReadyEvent());
		WaitForSingleObjectEx(framebuffer->getReadyEvent(), INFINITE, FALSE);
	}
}

void DxSwapchain::releaseFrame()
{
}

void DxSwapchain::present()
{
	HRESULT r = m_swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}
