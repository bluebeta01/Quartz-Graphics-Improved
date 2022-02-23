#pragma once
#include <d3d12.h>
#include <memory>
#include "framebuffer.h"

class DxFramebuffer : public Framebuffer
{
public:
	DxFramebuffer(const FramebufferCreateInfo& createInfo);

	ID3D12CommandAllocator* getAllocator() const { return m_allocator; }
	ID3D12Fence* getFence() const { return m_fence; }
	HANDLE getReadyEvent() const { return m_readyEvent; }
	int getFenceValue() const { return m_fenceValue; }
	void incrementFenceValue() { m_fenceValue++; }
	bool readyForRender();
	void waitIfNotReady();

private:
	ID3D12CommandAllocator* m_allocator = nullptr;
	ID3D12Fence* m_fence = nullptr;
	HANDLE m_readyEvent = {};
	int m_fenceValue = 0;
};