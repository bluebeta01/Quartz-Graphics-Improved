#include "dxrender3d.h"
#include "dxframebuffer.h"
#include "dxtexture2d.h"
#include "dxdevice.h"
#include "dxpipeline.h"
#include "dxcbuffer.h"
#include "dxvbuffer.h"
#include <assert.h>

DxRender3D::DxRender3D(std::shared_ptr<Device> device) :
	Render3D(device)
{
	

}

void DxRender3D::beginFrame(std::shared_ptr<Framebuffer> framebuffer)
{
	m_currentFramebuffer = framebuffer;

	std::shared_ptr<DxFramebuffer> dxfb = std::static_pointer_cast<DxFramebuffer>(m_currentFramebuffer);

	if (m_commandList == nullptr)
	{
		((DxDevice*)m_device.get())->getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dxfb->getAllocator(), nullptr, IID_PPV_ARGS(&m_commandList));
		m_commandList->Close();
	}

	dxfb->getAllocator()->Reset();
	m_commandList->Reset(dxfb->getAllocator(), nullptr);
	

	DxTexture2D* rtTexture = ((DxTexture2D*)framebuffer->getTextures()[0].get());
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = ((DxDevice*)m_device.get())->m_cpuRtvHeap->getHandleOfIndex(rtTexture->getRtvHeapIndex());
	DxTexture2D* dsTexture = ((DxTexture2D*)framebuffer->getTextures()[1].get());
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = ((DxDevice*)m_device.get())->m_cpuDsvHeap->getHandleOfIndex(dsTexture->getDsvHeapIndex());

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtTexture->getTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DxRender3D::clearFrame()
{
	DxTexture2D* rtTexture = ((DxTexture2D*)m_currentFramebuffer->getTextures()[0].get());
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = ((DxDevice*)m_device.get())->m_cpuRtvHeap->getHandleOfIndex(rtTexture->getRtvHeapIndex());
	DxTexture2D* dsTexture = ((DxTexture2D*)m_currentFramebuffer->getTextures()[1].get());
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = ((DxDevice*)m_device.get())->m_cpuDsvHeap->getHandleOfIndex(dsTexture->getDsvHeapIndex());
	m_commandList->ClearRenderTargetView(rtvHandle, rtTexture->getClearValue().Color, 0, nullptr);
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, dsTexture->getDepthClearValue().DepthStencil.Depth, dsTexture->getDepthClearValue().DepthStencil.Stencil, 0, nullptr);
}

void DxRender3D::setViewport(float x, float y, float width, float height)
{
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(x, y, width, height);
	m_commandList->RSSetViewports(1, &viewport);
}

void DxRender3D::setScissorRect(int x, int y, int width, int height)
{
	CD3DX12_RECT scissorRect = CD3DX12_RECT(x, y, (LONG)width, (LONG)height);
	m_commandList->RSSetScissorRects(1, &scissorRect);
}

void DxRender3D::bindPipeline(std::shared_ptr<Pipeline> pipeline)
{
	m_currentPipeline = pipeline;
	DxPipeline* dxpipeline = (DxPipeline*)m_currentPipeline.get();
	m_tablesStartingIndex = ((DxDevice*)m_device.get())->m_gpuCbvSrvUavHeap->requestSpace(dxpipeline->getShaderBindableDescriptions().size());
	m_commandList->SetGraphicsRootSignature(dxpipeline->getRootSignature());
	m_commandList->SetPipelineState(dxpipeline->getPipelineState());
	ID3D12DescriptorHeap* heaps[] = { ((DxDevice*)m_device.get())->m_gpuCbvSrvUavHeap->getHeap() };
	m_commandList->SetDescriptorHeaps(1, heaps);

	
}

void DxRender3D::bindCBuffer(std::shared_ptr<CBuffer> cbuffer, int tableIndex)
{
	DxCBuffer* cb = (DxCBuffer*)cbuffer.get();
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHeapHandle = ((DxDevice*)m_device.get())->m_cpuCbvSrvUavHeap->getHandleOfIndex(cb->getHeapIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE cbvGpuHeapHandle = ((DxDevice*)m_device.get())->m_gpuCbvSrvUavHeap->getCPUHandleOfIndex(m_tablesStartingIndex + tableIndex);

	((ID3D12Device*)m_device->getNativeResource())->CopyDescriptorsSimple(1, cbvGpuHeapHandle, cbvCpuHeapHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DxRender3D::bindTexture(std::shared_ptr<Texture2D> texture, int tableIndex)
{
	DxTexture2D* dxtex = (DxTexture2D*)texture.get();
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHeapHandle = ((DxDevice*)m_device.get())->m_cpuCbvSrvUavHeap->getHandleOfIndex(dxtex->getSrvHeapIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE srvGpuHeapHandle = ((DxDevice*)m_device.get())->m_gpuCbvSrvUavHeap->getCPUHandleOfIndex(m_tablesStartingIndex + m_currentPipeline->getCBufferCount() + tableIndex);

	((ID3D12Device*)m_device->getNativeResource())->CopyDescriptorsSimple(1, srvGpuHeapHandle, srvCpuHeapHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DxRender3D::bindResources()
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvTableHandle = ((DxDevice*)m_device.get())->m_gpuCbvSrvUavHeap->getGPUHandleOfIndex(m_tablesStartingIndex);
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvTableHandle = ((DxDevice*)m_device.get())->m_gpuCbvSrvUavHeap->getGPUHandleOfIndex(m_tablesStartingIndex + m_currentPipeline->getCBufferCount());

	DxPipeline* pipeline = (DxPipeline*)m_currentPipeline.get();

	if(pipeline->getCBufferCount() > 0)
		m_commandList->SetGraphicsRootDescriptorTable(pipeline->getCBVRootIndex(), cbvTableHandle);
	if(pipeline->getTextureCount() > 0)
		m_commandList->SetGraphicsRootDescriptorTable(pipeline->getSRVRootIndex(), srvTableHandle);
}

void DxRender3D::renderVBuffer(std::shared_ptr<VBuffer> vBuffer)
{
	D3D12_VERTEX_BUFFER_VIEW view = ((DxVBuffer*)vBuffer.get())->getBufferView();
	m_commandList->IASetVertexBuffers(0, 1, &view);
	m_commandList->DrawInstanced(vBuffer->getVertexCount(), 1, 0, 0);
}

void DxRender3D::endFrame()
{
	DxTexture2D* rt = (DxTexture2D*)m_currentFramebuffer->getTextures()[0].get();
	DxFramebuffer* dxfb = (DxFramebuffer*)m_currentFramebuffer.get();
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rt->getTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	HRESULT r = m_commandList->Close();
	assert(r == S_OK);

	ID3D12CommandList* renderCommandList[] = { m_commandList };
	((DxDevice*)m_device.get())->getQueue()->ExecuteCommandLists(1, renderCommandList);
	dxfb->incrementFenceValue();
	r = ((DxDevice*)m_device.get())->getQueue()->Signal(dxfb->getFence(), dxfb->getFenceValue());
	assert(r == S_OK);
}

void DxRender3D::uploadTexture(std::shared_ptr<Texture2D> texture, void* data, int dataSize)
{
	DxTexture2D* tex = ((DxTexture2D*)texture.get());
	tex->getSubresourceData().RowPitch = tex->getRowPitch();
	tex->getSubresourceData().SlicePitch = tex->getSlicePitch();
	tex->getSubresourceData().pData = data;
	
	UpdateSubresources(m_commandList, tex->getTexture(), tex->getUploadBuffer(), 0, 0, 1, &tex->getSubresourceData());

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(tex->getTexture(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

}

void DxRender3D::transitionViewportTextureForPresent(std::shared_ptr<Texture2D> texture)
{
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(((DxTexture2D*)texture.get())->getTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

void DxRender3D::transitionViewportTextureForRenderPas(std::shared_ptr<Texture2D> texture)
{
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(((DxTexture2D*)texture.get())->getTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT));
}

std::shared_ptr<void> DxRender3D::uploadViewportDescriptor(std::shared_ptr<Texture2D> viewportTexture)
{
	DxDevice* device = (DxDevice*)m_device.get();
	DxTexture2D* dxtex = (DxTexture2D*)viewportTexture.get();

	int gpuHeapIndex = device->m_gpuCbvSrvUavHeap->requestSpace(1);
	D3D12_CPU_DESCRIPTOR_HANDLE viewportFramebufferCpuHandle = device->m_cpuCbvSrvUavHeap->getHandleOfIndex(dxtex->getSrvHeapIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE viewportFramebufferCpuHandleGpu = device->m_gpuCbvSrvUavHeap->getCPUHandleOfIndex(gpuHeapIndex);
	device->getDevice()->CopyDescriptorsSimple(1, viewportFramebufferCpuHandleGpu, viewportFramebufferCpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	std::shared_ptr<D3D12_GPU_DESCRIPTOR_HANDLE> gpuHandle = std::make_shared<D3D12_GPU_DESCRIPTOR_HANDLE>(device->m_gpuCbvSrvUavHeap->getGPUHandleOfIndex(gpuHeapIndex));
	return std::static_pointer_cast<void>(gpuHandle);
}
