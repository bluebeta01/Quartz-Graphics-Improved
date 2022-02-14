#pragma once
#include <memory>
#include "framebuffer.h"
#include "vbuffer.h"
#include "cbuffer.h"
#include "texture2d.h"

class Render3D
{
public:
	Render3D(std::shared_ptr<Device> device) :
		m_device(device)
	{}

	virtual void beginFrame(std::shared_ptr<Framebuffer> framebuffer) = 0;
	virtual void clearFrame() = 0;
	virtual void renderVBuffer(std::shared_ptr<VBuffer> vBuffer, const std::vector<CBuffer>& cBuffers,
		const std::vector<NamedTexture>& textures) = 0;
	virtual void endFrame() = 0;
	virtual void uploadTexture(std::shared_ptr<Texture2D> texture) = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }

private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<Framebuffer> m_currentFramebuffer;
};