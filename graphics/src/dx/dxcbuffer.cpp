#include "dxcbuffer.h"
#include "d3dx12.h"
#include "dxdevice.h"

DxCBuffer::DxCBuffer(const CBufferCreateInfo& createInfo) :
	CBuffer(createInfo)
{
	m_size = (m_size + 255) & ~255;
	((ID3D12Device*)m_device->getNativeResource())->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_buffer)
	);

	std::shared_ptr<DxDevice> device = std::static_pointer_cast<DxDevice>(m_device);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_buffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_size;

	m_heapIndex = device->m_cpuCbvSrvUavHeap->getNextFreeIndex();
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = device->m_cpuCbvSrvUavHeap->getHandleOfIndex(m_heapIndex);
	device->getDevice()->CreateConstantBufferView(&cbvDesc, handle);

	CD3DX12_RANGE readRange(0, 0);
	m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferPtr));
}

void DxCBuffer::bufferData(void* data, int dataSize)
{
	memcpy_s(m_bufferPtr, m_size, data, dataSize);
}