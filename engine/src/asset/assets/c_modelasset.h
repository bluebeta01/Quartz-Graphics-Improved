#pragma once
#include <vector>
#include <memory>
#include <graphics.h>
#include "asset/assets/c_asset.h"
#include "asset/assets/c_materialasset.h"
#include "common.h"

namespace Quartz
{

struct ModelData
{
	int qmfVersion = 0;
	std::string modelName;
	std::string materialName;
	int vertCount = 0;
	int floatsPerVert = 0;
	std::shared_ptr<std::vector<float>> rawModelData;
};

class ModelAsset : public Asset
{
public:
	ModelData m_modelData;
	std::shared_ptr<MaterialAsset> m_material;
	std::shared_ptr<VBuffer> m_vBuffer;

	ModelAsset(const std::string& name, const std::string& absPath) : Asset(Asset::Type::MODEL, name, absPath), m_modelData({}) {}
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArguments);
	bool childrenLoaded();
	void loadDependencies();
};
}