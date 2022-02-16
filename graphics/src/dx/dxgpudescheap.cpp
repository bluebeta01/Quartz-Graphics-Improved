#include "dxgpudescheap.h"

DxGPUDescriptorHeap::DxGPUDescriptorHeap(ID3D12Device* device, DxDescriptorHeapType type, int size) :
	m_device(device), m_type(type), m_size(size)
{
	if (m_type == DxDescriptorHeapType::CBV_SRV_UAV)
		m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (m_type == DxDescriptorHeapType::DSV)
		m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (m_type == DxDescriptorHeapType::RTV)
		m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = m_size;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
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

CD3DX12_CPU_DESCRIPTOR_HANDLE DxGPUDescriptorHeap::getCPUHandleOfIndex(int index) const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heap->GetCPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DxGPUDescriptorHeap::getGPUHandleOfIndex(int index) const 
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heap->GetGPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
}
