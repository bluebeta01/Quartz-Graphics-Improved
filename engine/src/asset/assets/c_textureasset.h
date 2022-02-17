#pragma once
#include <vector>
#include <memory>
#include <graphics.h>
#include "asset/assets/c_asset.h"
#include "render/c_renderer.h"

namespace Quartz
{
struct TextureData
{
	unsigned int textureWidth = 0;
	unsigned int textureHeight = 0;
	std::shared_ptr<std::vector<unsigned char>> rawTextureData;
};

class TextureAsset : public Asset
{
public:
	unsigned int m_width;
	unsigned int m_height;
	TextureData m_textureData;
	std::shared_ptr<Texture2D> m_texture2D;
	TextureAsset(const std::string& name, const std::string& abolsutePath);
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
	bool childrenLoaded();
};
}