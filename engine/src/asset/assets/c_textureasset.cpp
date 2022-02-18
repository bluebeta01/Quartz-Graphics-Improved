#include "pch.h"
#include "asset/assets/c_textureasset.h"
#include "asset/c_assetmanager.h"
#include "render/c_renderer.h"

namespace Quartz
{
bool TextureAsset::dependenciesLoaded() const
{
	return m_loadStatus == AssetLoadStatus::LOADED;
}

void TextureAsset::clearRawData()
{
	m_rawData = nullptr;
}

void TextureAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(TextureAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
}

void TextureAsset::loadCallback(std::shared_ptr<void> callbackArgs)
{
	std::shared_ptr<TextureAsset> asset = std::static_pointer_cast<TextureAsset>(callbackArgs);

	asset->m_rawData = std::make_shared<std::vector<unsigned char>>();
	unsigned int errorCode = lodepng::decode(*asset->m_rawData.get(), (unsigned int&)asset->m_width, (unsigned int&)asset->m_height, asset->m_absolutePath);
	if (errorCode)
	{
		QZERROR("Failed to load texture \"{0}\"", asset->m_name);
		return;
	}

	Texture2DCreateInfo info = {};
	info.arraySize = 1;
	info.device = Renderer::s_device;
	info.height = asset->m_height;
	info.width = asset->m_width;
	info.type = TextureType::IMAGE;
	asset->m_texture2D = Texture2D::create(info);

	asset->m_loadStatus = AssetLoadStatus::READY_FOR_GPU_UPLOAD;
}

void TextureAsset::loadDependencies()
{
}
}