#pragma once
#include <memory>
#include "framebuffer.h"
#include "vbuffer.h"
#include "cbuffer.h"
#include "texture2d.h"
#include "pipeline.h"

class Render3D
{
public:
	Render3D(std::shared_ptr<Device> device) :
		m_device(device)
	{}

	static std::shared_ptr<Render3D> create(std::shared_ptr<Device> device);
	virtual void beginFrame(std::shared_ptr<Framebuffer> framebuffer) = 0;
	virtual void clearFrame() = 0;
	virtual void setViewport(float x, float y, float width, float height) = 0;
	virtual void setScissorRect(int x, int y, int width, int height) = 0;
	virtual void bindPipeline(std::shared_ptr<Pipeline> pipeline) = 0;
	virtual void bindCBuffer(std::shared_ptr<CBuffer> cbuffer, int tableIndex) = 0;
	virtual void bindTexture(std::shared_ptr<Texture2D> texture, int tableIndex) = 0;
	virtual void bindResources() = 0;
	virtual void renderVBuffer(std::shared_ptr<VBuffer> vBuffer) = 0;
	virtual void endFrame() = 0;
	virtual void uploadTexture(std::shared_ptr<Texture2D> texture, void* data, int dataSize) = 0;
	virtual void transitionViewportTextureForPresent(std::shared_ptr<Texture2D> texture) = 0;
	virtual void transitionViewportTextureForRenderPas(std::shared_ptr<Texture2D> texture) = 0;
	virtual std::shared_ptr<void> uploadViewportDescriptor(std::shared_ptr<Texture2D> viewportTexture) = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }

protected:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<Framebuffer> m_currentFramebuffer;
	std::shared_ptr<Pipeline> m_currentPipeline;
};