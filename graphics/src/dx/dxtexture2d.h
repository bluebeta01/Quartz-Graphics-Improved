#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "texture2d.h"

class DxTexture2D : public Texture2D
{
public:
	DxTexture2D(const Texture2DCreateInfo& createInfo);
	~DxTexture2D();
	void release();

	NativeResource getNativeResource() const;
	LONG_PTR getRowPitch() const;
	LONG_PTR getSlicePitch() const;
	void createReadbackBuffer();

	ID3D12Resource* getTexture() const { return m_texture; }
	ID3D12Resource* getUploadBuffer() const { return m_uploadBuffer; }
	ID3D12Resource* getReadbackBuffer() const { return m_readbackBuffer; }
	void setTexture(ID3D12Resource* texture) { m_texture = texture; }
	int getRtvHeapIndex() const { return m_rtvHeapIndex; }
	int getDsvHeapIndex() const { return m_dsvHeapIndex; }
	int getSrvHeapIndex() const { return m_srvHeapIndex; }
	D3D12_SUBRESOURCE_DATA& getSubresourceData() { return m_subresourceData; }
	D3D12_CLEAR_VALUE getClearValue() const { return m_clearValue; }
	D3D12_CLEAR_VALUE getDepthClearValue() const { return m_depthClearValue; }

private:
	ID3D12Resource* m_texture = nullptr;
	ID3D12Resource* m_uploadBuffer = nullptr;
	ID3D12Resource* m_readbackBuffer = nullptr;
	D3D12_RESOURCE_DESC m_resourceDesc = {};
	int m_rtvHeapIndex = -1;
	int m_dsvHeapIndex = -1;
	int m_srvHeapIndex = -1;
	D3D12_SUBRESOURCE_DATA m_subresourceData = {};
	D3D12_CLEAR_VALUE m_clearValue = {};
	D3D12_CLEAR_VALUE m_depthClearValue = {};
};