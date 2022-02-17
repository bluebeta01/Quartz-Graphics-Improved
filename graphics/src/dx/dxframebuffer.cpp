#include "dxframebuffer.h"
#include "dxdevice.h"
#include <assert.h>

DxFramebuffer::DxFramebuffer(const FramebufferCreateInfo& createInfo) :
	Framebuffer(createInfo)
{
	((ID3D12Device*)m_device->getNativeResource())->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	((ID3D12Device*)m_device->getNativeResource())->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocator));
	m_readyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}
