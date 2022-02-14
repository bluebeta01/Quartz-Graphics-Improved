#pragma once
#include "asset/assets/c_asset.h"

namespace Quartz
{
class TextAsset : public Asset
{
public:
	std::string m_text;
	TextAsset(const std::string& name, const std::string& abolsutePath) : Asset(Asset::Type::TEXT, name, abolsutePath) {};
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
};
}