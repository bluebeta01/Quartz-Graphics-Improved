#pragma once
#include <d3d12.h>
#include "cbuffer.h"

class DxCBuffer : public CBuffer
{
public:
	DxCBuffer(const CBufferCreateInfo& createInfo);

	void bufferData(void* data, int dataSize);
	NativeResource getNativeResource() const { return m_buffer; }

	int getHeapIndex() const { return m_heapIndex; }

private:
	ID3D12Resource* m_buffer = nullptr;
	UINT8* m_bufferPtr = nullptr;
	int m_heapIndex = 0;
};