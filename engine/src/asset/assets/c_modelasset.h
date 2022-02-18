#pragma once
#include <vector>
#include <memory>
#include <graphics.h>
#include "asset/assets/c_asset.h"
#include "asset/assets/c_materialasset.h"
#include "common.h"

namespace Quartz
{
class AssetManager;

class ModelAsset : public Asset
{
public:
	ModelAsset(const std::string& name, const std::string& absPath) :
		Asset(AssetType::MODEL, name, absPath)
	{}

	void clearRawData();
	bool dependenciesLoaded() const;

	std::shared_ptr<std::vector<float>> getRawData() { return m_rawData; }
	std::shared_ptr<VBuffer> getVBuffer() const { return m_vBuffer; }
	std::shared_ptr<MaterialAsset> getMaterial() const { return m_material; }

private:
	int m_fileVersion = 0;
	std::string m_modelName;
	std::string m_materialName;
	int m_vertexCount = 0;
	int m_vertexSize = 0;
	std::shared_ptr<std::vector<float>> m_rawData;
	std::shared_ptr<MaterialAsset> m_material;
	std::shared_ptr<VBuffer> m_vBuffer;

	void load(std::shared_ptr<Asset> asset);
	void loadDependencies();
	static void loadCallback(std::shared_ptr<void> callbackArguments);

	friend class AssetManager;
};
}