#pragma once
#include <d3d12.h>
#include <memory>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "device.h"
#include "dxcpudescheap.h"
#include "dxgpudescheap.h"

class DxDevice : public Device
{
public:
	DxDevice();

	std::shared_ptr<DxCPUDescriptorHeap> m_cpuCbvSrvUavHeap;
	std::shared_ptr<DxCPUDescriptorHeap> m_cpuDsvHeap;
	std::shared_ptr<DxCPUDescriptorHeap> m_cpuRtvHeap;
	std::shared_ptr<DxGPUDescriptorHeap> m_gpuCbvSrvUavHeap;

	NativeResource getNativeResource() const { return m_device; }
	void waitForIdle();

	ID3D12Device* getDevice() const { return m_device; }
	IDXGIFactory4* getFactory() const { return m_dxFactory4; }
	ID3D12CommandQueue* getQueue() const { return m_dxCommandQueue; }

private:
	ID3D12Device* m_device = nullptr;
	IDXGIFactory4* m_dxFactory4 = nullptr;
	IDXGIAdapter1* m_dxHardwareAdapater1 = nullptr;
	ID3D12Debug* m_dxDebugController = nullptr;
	ID3D12CommandQueue* m_dxCommandQueue = nullptr;
	ID3D12Fence* m_dxIdleFence = nullptr;
	HANDLE m_dxIdleFenceEvent = {};

	void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);
};