#pragma once
#include <memory>
#include <d3d12.h>
#include "vbuffer.h"

class DxVBuffer : public VBuffer
{
public:
	DxVBuffer(const VBufferCreateInfo& createInfo);

	void bufferData(void* data, int dataSize);

	NativeResource getNativeResource() const { return (NativeResource)m_buffer; }
	ID3D12Resource* getBuffer() const { return m_buffer; }
	const D3D12_VERTEX_BUFFER_VIEW& getBufferView() const { return m_bufferView; }

private:
	ID3D12Resource* m_buffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_bufferView = {};
};