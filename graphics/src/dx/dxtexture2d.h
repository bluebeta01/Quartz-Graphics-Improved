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
	void setTexture(ID3D12Resource* texture) { m_texture = texture; }

private:
	ID3D12Resource* m_texture = nullptr;
	ID3D12Resource* m_uploadBuffer = nullptr;
	D3D12_SUBRESOURCE_DATA m_textureData = {};
	int m_heapIndex = 0;
};