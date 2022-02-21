#pragma once
#include <unordered_map>
#include <graphics.h>
#include "asset/assets/c_asset.h"
#include "render/materialproperty.h"
#include "asset\assets\c_shaderasset.h"

namespace Quartz
{
class AssetManager;

class MaterialAsset : public Asset
{
public:
	MaterialAsset(const std::string& name, const std::string& abolsutePath) :
		Asset(AssetType::MATERIAL, name, abolsutePath)
	{}

	bool dependenciesLoaded() const;

	std::shared_ptr<ShaderAsset> getShader() const { return m_shader; }
	void setShader(std::shared_ptr<ShaderAsset> shader) { m_shader = shader; }
	void setProperty(const MaterialProperty& prop);
	void removeProperty(const std::string& propertyName);
	const MaterialProperty* getProperty(const std::string& propertyName);

private:
	std::shared_ptr<ShaderAsset> m_shader;
	std::string m_shaderName;
	std::unordered_map<std::string, MaterialProperty> m_properties;
	void load(std::shared_ptr<Asset> asset);
	static void loadCallback(std::shared_ptr<void> callbackArgs);
	void loadDependencies();

	friend class AssetManager;
};
}