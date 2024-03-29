#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "render3d.h"

class DxRender3D : public Render3D
{
public:
	DxRender3D(std::shared_ptr<Device> device);

	void beginFrame(std::shared_ptr<Framebuffer> framebuffer);
	void clearFrame();
	void setViewport(float x, float y, float width, float height);
	void setScissorRect(int x, int y, int width, int height);
	void bindPipeline(std::shared_ptr<Pipeline> pipeline);
	void bindCBuffer(std::shared_ptr<CBuffer> cbuffer, int tableIndex);
	void bindTexture(std::shared_ptr<Texture2D> texture, int tableIndex);
	void bindResources();
	void renderVBuffer(std::shared_ptr<VBuffer> vBuffer);
	void endFrame();
	void uploadTexture(std::shared_ptr<Texture2D> texture, void* data, int dataSize);
	void transitionViewportTextureForPresent(std::shared_ptr<Texture2D> texture);
	void transitionViewportTextureForRenderPas(std::shared_ptr<Texture2D> texture);
	std::shared_ptr<void> uploadViewportDescriptor(std::shared_ptr<Texture2D> viewportTexture);

	ID3D12GraphicsCommandList* getCommandList() { return m_commandList; }

private:
	ID3D12GraphicsCommandList* m_commandList = nullptr;
	CD3DX12_VIEWPORT m_viewport = {};
	CD3DX12_RECT m_scissorRect = {};
	int m_tablesStartingIndex = 0;
};