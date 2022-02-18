#pragma once
#include <vector>
#include <memory>
#include <graphics.h>
#include "asset/assets/c_asset.h"
#include "render/c_renderer.h"

namespace Quartz
{
class AssetManager;

class TextureAsset : public Asset
{
public:
	TextureAsset(const std::string& name, const std::string& abolsutePath) :
		Asset(AssetType::TEXTURE2D, name, abolsutePath)
	{}

	bool dependenciesLoaded() const;
	void clearRawData();

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	std::shared_ptr<std::vector<unsigned char>> getRawData() { return m_rawData; }
	std::shared_ptr<Texture2D> getTexture() const { return m_texture2D; }

private:
	std::shared_ptr<std::vector<unsigned char>> m_rawData;
	std::shared_ptr<Texture2D> m_texture2D;
	int m_width = 0;
	int m_height = 0;
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
	void loadDependencies();

	friend class AssetManager;
};
}