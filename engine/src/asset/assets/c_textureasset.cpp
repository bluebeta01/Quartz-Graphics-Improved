#include "pch.h"
#include "asset/assets/c_textureasset.h"
#include "asset/c_assetmanager.h"
#include "render/c_renderer.h"

namespace Quartz
{

TextureAsset::TextureAsset(const std::string& name, const std::string& abolsutePath) :
	Asset(Asset::Type::TEXTURE2D, name, abolsutePath), m_width(0), m_height(0), m_textureData({})
{
	//m_texture2D = Renderer::s_gengine.createTexture2D();
}

void TextureAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(TextureAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
}

void TextureAsset::loadCallback(std::shared_ptr<void> callbackArgs)
{
	std::shared_ptr<TextureAsset> asset = std::static_pointer_cast<TextureAsset>(callbackArgs);

	asset->m_textureData.rawTextureData = std::make_shared<std::vector<unsigned char>>();
	unsigned int errorCode = lodepng::decode(*asset->m_textureData.rawTextureData.get(), asset->m_textureData.textureWidth, asset->m_textureData.textureHeight, asset->m_absolutePath);
	if (!errorCode)
	{
		asset->m_height = asset->m_textureData.textureHeight;
		asset->m_width = asset->m_textureData.textureWidth;
	}
	else
	{
		QZERROR("Failed to load texture \"{0}\"", asset->m_name);
		return;
	}

	//asset->m_texture2D = Renderer::s_gengine.createTexture2D(asset->m_texture2D, asset->m_width, asset->m_height);
	//asset->m_texture2D->bufferData(asset->m_textureData.rawTextureData.get());
	Texture2DCreateInfo info = {};
	info.arraySize = 1;
	info.device = Renderer::s_device;
	info.height = asset->m_height;
	info.width = asset->m_width;
	info.type = TextureType::IMAGE;
	asset->m_texture2D = Texture2D::create(info);

	asset->m_loadStatus = Asset::Status::READY_FOR_GPU_UPLOAD;
}

bool TextureAsset::childrenLoaded()
{
	return m_loadStatus == Asset::Status::LOADED;
}
}