#include "dxvbuffer.h"
#include "dxdevice.h"

DxVBuffer::DxVBuffer(const VBufferCreateInfo& createInfo) :
	VBuffer(createInfo)
{
	((ID3D12Device*)m_device->getNativeResource())->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_buffer)
	);

	m_bufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
	m_bufferView.StrideInBytes = m_stride;
	m_bufferView.SizeInBytes = m_size;
}

void DxVBuffer::bufferData(void* data, int dataSize)
{
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, data, dataSize);
	m_buffer->Unmap(0, nullptr);
}
