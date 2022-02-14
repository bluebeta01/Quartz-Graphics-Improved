#include "framebuffer.h"

Framebuffer::Framebuffer(int textureCount)
{
	m_textures.resize(textureCount);
}

void Framebuffer::setTexture(int textureIndex, std::shared_ptr<Texture2D> texture)
{
	m_textures[textureIndex] = texture;
}