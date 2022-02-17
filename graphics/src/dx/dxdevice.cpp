#include "dxdevice.h"

DxDevice::DxDevice()
{
    bool dxDebug = true;
    UINT factoryFlags = 0;
    if (dxDebug)
    {
        D3D12GetDebugInterface(IID_PPV_ARGS(&m_dxDebugController));
        m_dxDebugController->EnableDebugLayer();
        factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
    CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_dxFactory4));
    GetHardwareAdapter(m_dxFactory4, &m_dxHardwareAdapater1);
    D3D12CreateDevice(m_dxHardwareAdapater1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));


    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_dxCommandQueue));


    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_dxIdleFence));
    m_dxIdleFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    m_cpuCbvSrvUavHeap = std::make_shared<DxCPUDescriptorHeap>(m_device, DxDescriptorHeapType::CBV_SRV_UAV);
    m_cpuDsvHeap = std::make_shared<DxCPUDescriptorHeap>(m_device, DxDescriptorHeapType::DSV);
    m_cpuRtvHeap = std::make_shared<DxCPUDescriptorHeap>(m_device, DxDescriptorHeapType::RTV);
    m_gpuCbvSrvUavHeap = std::make_shared<DxGPUDescriptorHeap>(m_device, DxDescriptorHeapType::CBV_SRV_UAV, 1000);
}

void DxDevice::waitForIdle()
{
    m_dxCommandQueue->Signal(m_dxIdleFence, 1);
    m_dxIdleFence->SetEventOnCompletion(1, m_dxIdleFenceEvent);
    WaitForSingleObjectEx(m_dxIdleFenceEvent, INFINITE, FALSE);
    m_dxIdleFence->Signal(0);
}

void DxDevice::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
{
    *ppAdapter = nullptr;
    for (UINT adapterIndex = 0; ; ++adapterIndex)
    {
        IDXGIAdapter1* pAdapter = nullptr;
        if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
        {
            // No more adapters to enumerate.
            break;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            *ppAdapter = pAdapter;
            return;
        }
        pAdapter->Release();
    }
}