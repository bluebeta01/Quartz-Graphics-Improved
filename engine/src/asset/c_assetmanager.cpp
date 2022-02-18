#include "pch.h"
#include "asset/c_assetmanager.h"
#include "asset/filesystem/c_filestream.h"
#include "asset/assets/c_textasset.h"
#include "render/c_renderer.h"

namespace Quartz
{
std::vector<std::shared_ptr<Plugin>> AssetManager::s_plugins;
std::unordered_map<std::string, std::shared_ptr<Asset>> AssetManager::s_assets;
std::list<std::shared_ptr<Asset>> AssetManager::s_assetsInProgress;
ThreadPool AssetManager::s_threadPool;

AssetManager::AssetManager()
{

}

void AssetManager::initialize()
{
	loadPlugins();
	s_threadPool.initialize(2);
	QZINFO("Asset manager thread count: {0}", s_threadPool.m_threadCount);
}

void AssetManager::update()
{
	proccessAssets();
}

void AssetManager::deinitialize()
{
	s_threadPool.deinitialize();
}

void AssetManager::loadPlugins()
{
	FileStream stream;
	stream.loadFile("plugins/plugins.txt", true);
	std::string text = stream.readAllText();
	std::stringstream ss(text);
	std::string pluginName;
	while (std::getline(ss, pluginName, '\n'))
	{
		std::shared_ptr<Plugin> plugin = std::make_shared<Plugin>();
		plugin->m_name = pluginName;
		s_plugins.push_back(plugin);
		QZINFO("Registering plugin \"{0}\"", pluginName);
	}
	if (s_plugins.size() == 0)
		QZERROR("No plugins were loaded!");
}

std::string AssetManager::findFile(const std::string& path)
{
	for (std::shared_ptr<Plugin> p : s_plugins)
	{
		if (p->containsFile(path))
		{
			std::string absolutePath = "plugins/";
			absolutePath.append(p->m_name);
			absolutePath.append("/");
			absolutePath.append(path);
			return absolutePath;
		}
	}
	return "";
}

std::shared_ptr<Asset> AssetManager::getAsset(AssetType type, const std::string& pluginRelativePath)
{
	auto i = s_assets.find(pluginRelativePath);
	if (i != s_assets.end())
		return i->second;


	std::string absPath = findFile(pluginRelativePath.c_str());
	if (absPath == "")
	{
		QZERROR("Asset \"{0}\" could not be found in any loaded plugin", pluginRelativePath);
		return std::shared_ptr<Asset>(nullptr);
	}

	//Asset loading dispatch
	switch (type)
	{
	case AssetType::TEXT:
	{
		std::shared_ptr<TextAsset> asset = std::make_shared<TextAsset>(pluginRelativePath, absPath);
		s_assets[pluginRelativePath] = asset;
		s_assetsInProgress.push_back(asset);
		asset->load(asset);
		return asset;
		break;
	}

	case AssetType::TEXTURE2D:
	{
		std::shared_ptr<TextureAsset> asset = std::make_shared<TextureAsset>(pluginRelativePath, absPath);
		s_assets[pluginRelativePath] = asset;
		s_assetsInProgress.push_back(asset);
		asset->load(asset);
		return asset;
		break;
	}

	case AssetType::MATERIAL:
	{
		std::shared_ptr<MaterialAsset> asset = std::make_shared<MaterialAsset>(pluginRelativePath, absPath);
		s_assets[pluginRelativePath] = asset;
		s_assetsInProgress.push_back(asset);
		asset->load(asset);
		return asset;
		break;
	}

	case AssetType::MODEL:
	{
		std::shared_ptr<ModelAsset> asset = std::make_shared<ModelAsset>(pluginRelativePath, absPath);
		s_assets[pluginRelativePath] = asset;
		s_assetsInProgress.push_back(asset);
		asset->load(asset);
		return asset;
		break;
	}

	case AssetType::SHADER:
	{
		std::shared_ptr<ShaderAsset> asset = std::make_shared<ShaderAsset>(pluginRelativePath, absPath);
		s_assets[pluginRelativePath] = asset;
		s_assetsInProgress.push_back(asset);
		asset->load(asset);
		return asset;
		break;
	}

	default:
	{
		return nullptr;
		break;
	}
	}
}

void AssetManager::proccessAssets()
{
	auto iter = s_assetsInProgress.begin();
	int size = s_assetsInProgress.size();
	int i = 0;
	while (iter != s_assetsInProgress.end() && i < size)
	{
		i++;
		std::shared_ptr<Asset> asset = *iter;
		
		if (asset->m_hasDependencies)
		{
			asset->m_hasDependencies = false;
			asset->loadDependencies();
		}

		if (asset->m_loadStatus == AssetLoadStatus::LOADED)
		{
			iter = s_assetsInProgress.erase(iter);
			continue;
		}

		if (asset->m_loadStatus == AssetLoadStatus::READY_FOR_GPU_UPLOAD)
		{
			//TODO: VBuffer uploading isn't scheduled on a command list. It's done here and now,
			//	which takes a while, so we return in this statement. Change this in future
			if(Renderer::uploadAsset(asset))
				asset->m_loadStatus = AssetLoadStatus::GPU_UPLOAD_REQUESTED;
			iter++;
			return;
		}

		if (asset->m_loadStatus == AssetLoadStatus::GPU_UPLOAD_REQUESTED)
		{
			if (asset->m_type == AssetType::MODEL)
			{
				std::shared_ptr<ModelAsset> model = std::static_pointer_cast<ModelAsset>(asset);
				model->m_loadStatus = AssetLoadStatus::LOADED;
				model->clearRawData();
				iter = s_assetsInProgress.erase(iter);
				continue;
			}

			if (asset->m_type == AssetType::TEXTURE2D)
			{
				std::shared_ptr<TextureAsset> texture = std::static_pointer_cast<TextureAsset>(asset);
				//TODO: Impliment a way to know when the texture is in GPU memory so we can safely call texture.clearRawData
				if (/*texture->m_texture2D->m_uploaded*/true)
				{
					texture->m_loadStatus = AssetLoadStatus::LOADED;
					//texture->m_textureData.rawTextureData = nullptr;
					iter = s_assetsInProgress.erase(iter);
					continue;
				}
				else
				{
					iter++;
					continue;
				}
			}
		}
	}
}

}