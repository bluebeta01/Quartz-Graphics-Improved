#pragma once
#include <memory>
#include <vector>
#include <list>
#include <d3d12.h>
#include "d3dx12.h"
#include "dxdescheap.h"

class DxCPUDescriptorHeap
{
public:
	DxCPUDescriptorHeap(ID3D12Device* device, DxDescriptorHeapType type);
	
	int getNextFreeIndex();
	void freeIndex(int index);
	ID3D12DescriptorHeap* getHeapOfIndex(int index);
	CD3DX12_CPU_DESCRIPTOR_HANDLE getHandleOfIndex(int index);

	DxDescriptorHeapType getType() const { return m_type; }
	UINT getDescriptorSize() const { return m_descriptorSize; }

private:
	ID3D12Device* m_device = nullptr;
	DxDescriptorHeapType m_type;
	std::vector<ID3D12DescriptorHeap*> m_heaps;
	UINT m_descriptorSize = 0;
	std::list<int> m_freeIndices;

	void createHeap();
};