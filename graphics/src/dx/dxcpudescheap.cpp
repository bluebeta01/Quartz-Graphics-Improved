#include "dxcpudescheap.h"
#include "dxdevice.h"

DxCPUDescriptorHeap::DxCPUDescriptorHeap(ID3D12Device* device, DxDescriptorHeapType type) :
	m_device(device), m_type(type)
{
	if (m_type == DxDescriptorHeapType::CBV_SRV_UAV)
		m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	if (m_type == DxDescriptorHeapType::DSV)
		m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (m_type == DxDescriptorHeapType::RTV)
		m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	createHeap();
}

int DxCPUDescriptorHeap::getNextFreeIndex()
{
	if (m_freeIndices.size() == 0)
		createHeap();

	int front = m_freeIndices.front();
	m_freeIndices.pop_front();
	return front;
}

void DxCPUDescriptorHeap::freeIndex(int index)
{
	m_freeIndices.push_back(index);
}

ID3D12DescriptorHeap* DxCPUDescriptorHeap::getHeapOfIndex(int index)
{
	int heapOffset = index % 100;
	int heapIndex = (index - heapOffset) / 100;
	return m_heaps[heapIndex];
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DxCPUDescriptorHeap::getHandleOfIndex(int index)
{
	int heapOffset = index % 100;
	int heapIndex = (index - heapOffset) / 100;

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_heaps[heapIndex]->GetCPUDescriptorHandleForHeapStart(), heapOffset, m_descriptorSize);
	return handle;
}

void DxCPUDescriptorHeap::createHeap()
{
	for (int i = 0; i < 100; i++)
		m_freeIndices.push_back((m_heaps.size() * 100) + i);

	ID3D12DescriptorHeap* heap;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 100;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	m_heaps.push_back(heap);
}
