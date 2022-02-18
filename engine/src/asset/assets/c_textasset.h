#pragma once
#include "asset/assets/c_asset.h"

namespace Quartz
{
class AssetManager;

class TextAsset : public Asset
{
public:
	TextAsset(const std::string& name, const std::string& abolsutePath) :
		Asset(AssetType::TEXT, name, abolsutePath)
	{}

	bool dependenciesLoaded() const;

	const std::string& getText() const { return m_text; }

private:
	std::string m_text;
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
	void loadDependencies();

	friend class AssetManager;
};
}