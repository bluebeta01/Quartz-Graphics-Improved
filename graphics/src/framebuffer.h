#pragma once
#include <vector>
#include <memory>
#include "texture2d.h"

struct FramebufferCreateInfo
{
	std::shared_ptr<Device> device;
	int textureCount;
	std::vector<std::shared_ptr<Texture2D>>* textures;
};

class Framebuffer
{
public:
	Framebuffer(const FramebufferCreateInfo& createInfo);
	virtual ~Framebuffer() {};

	static std::shared_ptr<Framebuffer> create(const FramebufferCreateInfo& createInfo);
	void setTexture(int textureIndex, std::shared_ptr<Texture2D> texture);
	virtual bool readyForRender() = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	const std::vector<std::shared_ptr<Texture2D>>& getTextures() const { return m_textures; }

protected:
	std::shared_ptr<Device> m_device;
	std::vector<std::shared_ptr<Texture2D>> m_textures;
};