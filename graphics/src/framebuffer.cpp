#include "framebuffer.h"

Framebuffer::Framebuffer(const FramebufferCreateInfo& createInfo) :
	m_device(createInfo.device), m_textures(*createInfo.textures)
{
	
}

void Framebuffer::setTexture(int textureIndex, std::shared_ptr<Texture2D> texture)
{
	m_textures[textureIndex] = texture;
}

void Framebuffer::releaseTextures()
{
	for (std::shared_ptr<Texture2D> texture : m_textures)
		texture->release();
}