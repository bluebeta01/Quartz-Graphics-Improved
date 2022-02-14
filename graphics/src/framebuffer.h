#pragma once
#include <vector>
#include <memory>
#include "texture2d.h"

class Framebuffer
{
public:
	Framebuffer(int textureCount);

	void setTexture(int textureIndex, std::shared_ptr<Texture2D> texture);

	const std::vector<std::shared_ptr<Texture2D>>& getTextures() const { return m_textures; }

private:
	std::vector<std::shared_ptr<Texture2D>> m_textures;
};