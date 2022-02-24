#include "dxtexture2d.h"
#include "dxdevice.h"
#include <iostream>

DxTexture2D::DxTexture2D(const Texture2DCreateInfo& createInfo) :
	Texture2D(createInfo)
{
	std::shared_ptr<DxDevice> device = std::static_pointer_cast<DxDevice>(m_device);

	m_clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_clearValue.Color[0] = 0;
	m_clearValue.Color[1] = 0.2;
	m_clearValue.Color[2] = 0.4f;
	m_clearValue.Color[3] = 1.0;

	m_depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	m_depthClearValue.DepthStencil.Depth = 1;
	m_depthClearValue.DepthStencil.Stencil = 0;

	if (createInfo.textureResource == nullptr)
	{
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.DepthOrArraySize = m_arraySize;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

		if (m_type == TextureType::IMAGE)
		{
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			state = D3D12_RESOURCE_STATE_COPY_DEST;
		}
		if (m_type == TextureType::DEPTH_STENCIL)
		{
			textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
		if (m_type == TextureType::RENDER_TARGET)
		{
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			state = D3D12_RESOURCE_STATE_PRESENT;
		}
			

		device->getDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			state,
			(m_type == TextureType::IMAGE ? nullptr : (m_type == TextureType::RENDER_TARGET ? &m_clearValue : &m_depthClearValue)),
			IID_PPV_ARGS(&m_texture)
		);
	}
	else
	{
		m_texture = (ID3D12Resource*)createInfo.textureResource;
	}

	if (m_type == TextureType::IMAGE)
	{
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture, 0, 1);

		device->getDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_uploadBuffer));
		
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (m_arraySize == 1)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
		}
		else
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = m_arraySize;
		}

		m_srvHeapIndex = device->m_cpuCbvSrvUavHeap->getNextFreeIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = device->m_cpuCbvSrvUavHeap->getHandleOfIndex(m_srvHeapIndex);
		device->getDevice()->CreateShaderResourceView(m_texture, &srvDesc, handle);
	}

	if (m_type == TextureType::DEPTH_STENCIL)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
		if (m_arraySize == 1)
			depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		else
		{
			depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			depthStencilDesc.Texture2DArray.ArraySize = m_arraySize;
		}

		m_dsvHeapIndex = device->m_cpuDsvHeap->getNextFreeIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = device->m_cpuDsvHeap->getHandleOfIndex(m_dsvHeapIndex);
		device->getDevice()->CreateDepthStencilView(m_texture, &depthStencilDesc, handle);
	}

	if (m_type == TextureType::RENDER_TARGET)
	{
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (m_arraySize == 1)
		{
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		}
		else
		{
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = m_arraySize;
		}

		m_rtvHeapIndex = device->m_cpuRtvHeap->getNextFreeIndex();
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = device->m_cpuRtvHeap->getHandleOfIndex(m_rtvHeapIndex);
		device->getDevice()->CreateRenderTargetView(m_texture, &desc, handle);

		if (createInfo.createSrvForRenderTarget)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			if (m_arraySize == 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.ArraySize = m_arraySize;
			}

			m_srvHeapIndex = device->m_cpuCbvSrvUavHeap->getNextFreeIndex();
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle = device->m_cpuCbvSrvUavHeap->getHandleOfIndex(m_srvHeapIndex);
			device->getDevice()->CreateShaderResourceView(m_texture, &srvDesc, handle);
		}
	}
}

DxTexture2D::~DxTexture2D()
{
}

void DxTexture2D::release()
{
	if (m_texture)
		m_texture->Release();
	if (m_uploadBuffer)
		m_uploadBuffer->Release();
	if (m_readbackBuffer)
		m_readbackBuffer->Release();

	DxDevice* device = (DxDevice*)m_device.get();
	if (m_rtvHeapIndex != -1)
		device->m_cpuRtvHeap->freeIndex(m_rtvHeapIndex);
	if (m_dsvHeapIndex != -1)
		device->m_cpuDsvHeap->freeIndex(m_dsvHeapIndex);
	if (m_srvHeapIndex != -1)
		device->m_cpuCbvSrvUavHeap->freeIndex(m_srvHeapIndex);
}

NativeResource DxTexture2D::getNativeResource() const
{
	return (NativeResource)m_texture;
}

LONG_PTR DxTexture2D::getRowPitch() const
{
	return m_width * 4;
}

LONG_PTR DxTexture2D::getSlicePitch() const
{
	return m_width * 4 * m_height;
}

void DxTexture2D::createReadbackBuffer()
{
	if (m_readbackBuffer)
		return;

	DxDevice* device = (DxDevice*)m_device.get();

	const UINT64 readbackBufferSize = GetRequiredIntermediateSize(m_texture, 0, 1);

	device->getDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(readbackBufferSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_readbackBuffer));
}
