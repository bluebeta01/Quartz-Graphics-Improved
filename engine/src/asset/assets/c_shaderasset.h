#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <graphics.h>
#include "common.h"
#include "asset/assets/c_asset.h"
#include "render/shaderproperty.h"

namespace Quartz
{
class ShaderAsset : public Asset
{
public:
	std::shared_ptr<Pipeline> m_pipeline;
	int m_id = 0;
	std::unordered_map<std::string, ShaderProperty> m_properties;
	ShaderAsset(const std::string& name, const std::string& absolutePath);
	void load(std::shared_ptr<Asset> asset);
	void setMatrixBuffer(const Transform& transform, unsigned int frameIndex);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
};
}