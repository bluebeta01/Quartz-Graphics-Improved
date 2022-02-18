#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <graphics.h>
#include "common.h"
#include "asset/assets/c_asset.h"

namespace Quartz
{
class AssetManager;

class ShaderAsset : public Asset
{
public:
	ShaderAsset(const std::string& name, const std::string& absolutePath);
	
	bool dependenciesLoaded() const;

	std::shared_ptr<Pipeline> getPipeline() const { return m_pipeline; }
	int getId() const { return m_id; }

private:
	std::shared_ptr<Pipeline> m_pipeline;
	int m_id = 0;
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
	void loadDependencies();

	friend class AssetManager;
};
}