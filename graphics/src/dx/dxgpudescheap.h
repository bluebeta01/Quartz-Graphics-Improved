#pragma once
#include <d3d12.h>
#include <memory>
#include "d3dx12.h"
#include "dxdescheap.h"

class DxGPUDescriptorHeap
{
public:
	DxGPUDescriptorHeap(ID3D12Device* device, DxDescriptorHeapType type, int size);

	int getNextIndex();
	int getCurrentIndex();
	int requestSpace(int numOfIndexes);
	D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandleOfIndex (int index) const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandleOfIndex (int index) const;

	DxDescriptorHeapType getType() const { return m_type; }
	ID3D12DescriptorHeap* getHeap() const { return m_heap; }
	UINT getDescriptorSize() const { return m_descriptorSize; }

private:
	ID3D12Device* m_device;
	DxDescriptorHeapType m_type = DxDescriptorHeapType::CBV_SRV_UAV;
	int m_size = 0;
	ID3D12DescriptorHeap* m_heap = nullptr;
	UINT m_descriptorSize = 0;
	int m_currentIndex = 0;
};