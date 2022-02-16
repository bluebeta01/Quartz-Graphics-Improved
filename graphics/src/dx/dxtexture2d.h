#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "texture2d.h"

class DxTexture2D : public Texture2D
{
public:
	DxTexture2D(const Texture2DCreateInfo& createInfo);

	NativeResource getNativeResource() const;
	LONG_PTR getRowPitch() const;
	LONG_PTR getSlicePitch() const;

	ID3D12Resource* getTexture() const { return m_texture; }
	ID3D12Resource* getUploadBuffer() const { return m_uploadBuffer; }
	void setTexture(ID3D12Resource* texture) { m_texture = texture; }
	int getHeapIndex() const { return m_heapIndex; }
	D3D12_SUBRESOURCE_DATA& getSubresourceData() { return m_subresourceData; }

private:
	ID3D12Resource* m_texture = nullptr;
	ID3D12Resource* m_uploadBuffer = nullptr;
	int m_heapIndex = 0;
	D3D12_SUBRESOURCE_DATA m_subresourceData = {};
};