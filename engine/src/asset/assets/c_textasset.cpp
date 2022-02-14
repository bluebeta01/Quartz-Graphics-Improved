#include "pch.h"
#include "asset/assets/c_textasset.h"
#include "asset/c_assetmanager.h"
#include "asset/filesystem/c_filestream.h"

namespace Quartz
{
void TextAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(TextAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
}
void TextAsset::loadCallback(std::shared_ptr<void> callbackArgs)
{
	std::shared_ptr<TextAsset> asset = std::static_pointer_cast<TextAsset>(callbackArgs);

	FileStream stream;
	stream.loadFile(asset->m_absolutePath.c_str(), true);
	asset->m_text = stream.readAllText();
	asset->m_loadStatus = Asset::Status::LOADED;
}
}