#pragma once
#include <unordered_map>
#include <graphicsengine.h>
#include "asset/assets/c_asset.h"

namespace Quartz
{
class MaterialAsset : public Asset
{
public:
	std::shared_ptr<Material> m_material;
	std::string m_shaderName;
	MaterialAsset(const std::string& name, const std::string& abolsutePath);
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
	bool childrenLoaded();
	void loadDependencies();

private:
};
}