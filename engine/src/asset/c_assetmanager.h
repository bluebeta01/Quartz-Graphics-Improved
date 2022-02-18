#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <list>
#include "c_threadpool.h"
#include "asset/filesystem/c_plugin.h"
#include "asset/assets/c_asset.h"
#include "asset/assets/c_textureasset.h"
#include "asset/assets/c_materialasset.h"
#include "asset/assets/c_modelasset.h"
#include "asset/assets/c_shaderasset.h"

namespace Quartz
{
class AssetManager
{
public:
	static ThreadPool s_threadPool;
	static void initialize();
	static void update();
	static void deinitialize();
	static std::string findFile(const std::string& path);
	static std::shared_ptr<Asset> getAsset(AssetType type, const std::string& pluginRelativePath);

private:
	static std::vector<std::shared_ptr<Plugin>> s_plugins;
	static std::unordered_map<std::string, std::shared_ptr<Asset>> s_assets;
	static std::list<std::shared_ptr<Asset>> s_assetsInProgress;
	AssetManager();
	static void loadPlugins();
	static void proccessAssets();
};
}