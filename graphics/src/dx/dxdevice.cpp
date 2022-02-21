#include "dxdevice.h"
#include "dxtexture2d.h"

DxDevice::DxDevice()
{
    bool dxDebug = true;
    UINT factoryFlags = 0;
    if (dxDebug)
    {
        ID3D12DeviceRemovedExtendedDataSettings* pDredSettings;

        D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings));
        pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

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

    m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_textureUploadCommandAllocator));
    m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_textureUploadCommandAllocator, nullptr, IID_PPV_ARGS(&m_textureUploadCommandList));
    m_textureUploadCommandList->Close();
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_textureUploadFence));

    m_cpuCbvSrvUavHeap = std::make_shared<DxCPUDescriptorHeap>(m_device, DxDescriptorHeapType::CBV_SRV_UAV);
    m_cpuDsvHeap = std::make_shared<DxCPUDescriptorHeap>(m_device, DxDescriptorHeapType::DSV);
    m_cpuRtvHeap = std::make_shared<DxCPUDescriptorHeap>(m_device, DxDescriptorHeapType::RTV);
    m_gpuCbvSrvUavHeap = std::make_shared<DxGPUDescriptorHeap>(m_device, DxDescriptorHeapType::CBV_SRV_UAV, 100000);
}

void DxDevice::waitForIdle()
{
    m_dxCommandQueue->Signal(m_dxIdleFence, 1);
    m_dxIdleFence->SetEventOnCompletion(1, m_dxIdleFenceEvent);
    WaitForSingleObjectEx(m_dxIdleFenceEvent, INFINITE, FALSE);
    m_dxIdleFence->Signal(0);
}

void DxDevice::uploadTexture(std::shared_ptr<Texture2D> texture, void* data, int dataSize)
{
    DxTexture2D* dxtex = (DxTexture2D*)texture.get();
    dxtex->getSubresourceData().pData = data;
    dxtex->getSubresourceData().RowPitch = dxtex->getRowPitch();
    dxtex->getSubresourceData().SlicePitch = dxtex->getSlicePitch();

    m_textureUploadCommandAllocator->Reset();
    m_textureUploadCommandList->Reset(m_textureUploadCommandAllocator, nullptr);
    UpdateSubresources(m_textureUploadCommandList, dxtex->getTexture(), dxtex->getUploadBuffer(), 0, 0, 1, &dxtex->getSubresourceData());
    m_textureUploadCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(dxtex->getTexture(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    m_textureUploadCommandList->Close();

    ID3D12CommandList* renderCommandList[] = { m_textureUploadCommandList };
    m_dxCommandQueue->ExecuteCommandLists(1, renderCommandList);
    m_textureUploadFenceValue++;
    m_dxCommandQueue->Signal(m_textureUploadFence, m_textureUploadFenceValue);
}

bool DxDevice::textureUploadReady()
{
    return m_textureUploadFence->GetCompletedValue() >= m_textureUploadFenceValue;
}

std::vector<uint8_t> DxDevice::readbackTexture(std::shared_ptr<Texture2D> texture)
{
    waitForIdle();
    m_textureUploadCommandAllocator->Reset();
    m_textureUploadCommandList->Reset(m_textureUploadCommandAllocator, nullptr);

    DxTexture2D* dxtex = (DxTexture2D*)texture.get();
    if (!dxtex->getReadbackBuffer())
        dxtex->createReadbackBuffer();

    m_textureUploadCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(dxtex->getTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE));
   
    CD3DX12_TEXTURE_COPY_LOCATION source(dxtex->getTexture(), 0);
    source.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    source.SubresourceIndex = 0;

    CD3DX12_SUBRESOURCE_FOOTPRINT destinationFootprint(DXGI_FORMAT_R8G8B8A8_UNORM, dxtex->getWidth(), dxtex->getHeight(), 1, dxtex->getRowPitch());
    CD3DX12_TEXTURE_COPY_LOCATION destination(dxtex->getReadbackBuffer());
    destination.Type = D3D12_TEXTURE_COPY_TYPE::D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    destination.PlacedFootprint.Footprint = destinationFootprint;

    m_textureUploadCommandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);

    m_textureUploadCommandList->Close();
    ID3D12CommandList* renderCommandList[] = { m_textureUploadCommandList };
    m_dxCommandQueue->ExecuteCommandLists(1, renderCommandList);
    waitForIdle();

    const UINT64 readbackBufferSize = GetRequiredIntermediateSize(dxtex->getTexture(), 0, 1);
    D3D12_RANGE readbackBufferRange{ 0, readbackBufferSize };
    std::vector<uint8_t> data(readbackBufferSize);
    void* dataPtr;

    dxtex->getReadbackBuffer()->Map(0, &readbackBufferRange, (void**)&dataPtr);

    memcpy_s(data.data(), data.size(), dataPtr, readbackBufferSize);

    D3D12_RANGE emptyRange{ 0, 0 };
    dxtex->getReadbackBuffer()->Unmap(0, &emptyRange);

    return data;
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
            DXGI_ADAPTER_DESC desc = {};
            pAdapter->GetDesc(&desc);
            *ppAdapter = pAdapter;
            return;
        }
        pAdapter->Release();
    }
}