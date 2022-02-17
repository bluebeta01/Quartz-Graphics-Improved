#include "dxgpudescheap.h"
#include <assert.h>

DxGPUDescriptorHeap::DxGPUDescriptorHeap(ID3D12Device* device, DxDescriptorHeapType type, int size) :
	m_device(device), m_type(type), m_size(size)
{
	m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = m_size;
	heapDesc.NodeMask = 0;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT r = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
	assert(r == S_OK);
}

int DxGPUDescriptorHeap::getNextIndex()
{
	int i = m_currentIndex;
	m_currentIndex = (m_currentIndex + 1) % m_size;
	return i;
}

int DxGPUDescriptorHeap::getCurrentIndex()
{
	return m_currentIndex;
}

int DxGPUDescriptorHeap::requestSpace(int numOfIndexes)
{
	if (m_currentIndex + numOfIndexes >= m_size)
		m_currentIndex = 0;

	int i = m_currentIndex;
	m_currentIndex += numOfIndexes;
	return i;
}

D3D12_CPU_DESCRIPTOR_HANDLE DxGPUDescriptorHeap::getCPUHandleOfIndex(int index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * m_descriptorSize;
	return handle;
	//return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heap->GetCPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DxGPUDescriptorHeap::getGPUHandleOfIndex(int index) const 
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
}
