#include "pch.h"
#include "asset/assets/c_materialasset.h"
#include "asset/c_assetmanager.h"
#include "asset/filesystem/c_filestream.h"

namespace Quartz
{
bool MaterialAsset::dependenciesLoaded() const
{
	bool loaded = m_loadStatus == AssetLoadStatus::LOADED;
	if (!loaded)
		return false;

	for (auto& prop : m_properties)
	{
		if (prop.second.m_type == MaterialProperty::Type::TEXTURE2D)
		{
			if (prop.second.m_value != nullptr)
				loaded &= std::static_pointer_cast<TextureAsset>(prop.second.m_value)->dependenciesLoaded();
			else
				return false;
		}
	}

	if (m_shader != nullptr)
		loaded &= m_shader->dependenciesLoaded();
	else
		return false;

	return loaded;
}

void MaterialAsset::setProperty(const MaterialProperty& prop)
{
	m_properties[prop.m_name] = prop;
}

void MaterialAsset::removeProperty(const std::string& propertyName)
{
	auto p = m_properties.find(propertyName);
	if (p == m_properties.end())
		return;

	m_properties.erase(p);
}

const MaterialProperty* MaterialAsset::getProperty(const std::string& propertyName)
{
	auto& p = m_properties.find(propertyName);
	if (p != m_properties.end())
		return &p->second;

	return nullptr;
}

void MaterialAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(MaterialAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
}

void MaterialAsset::loadCallback(std::shared_ptr<void> callbackArgs)
{
	std::shared_ptr<MaterialAsset> material = std::static_pointer_cast<MaterialAsset>(callbackArgs);

	FileStream stream;
	stream.loadFile(material->m_absolutePath.c_str(), true);
	std::string rawJson = stream.readAllText();

	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(rawJson.c_str());
	}
	catch (const std::exception&)
	{
		QZERROR("Failed to parse JSON for \"{0}\"", material->m_name);
		return;
	}



	nlohmann::json::string_t shaderName = json["shader"];
	nlohmann::json::array_t propsArray = json["props"];
	for (int i = 0; i < propsArray.size(); i++)
	{
		MaterialProperty materialProperty = {};
		nlohmann::json::string_t propType = propsArray[i]["type"];
		if (propType == "texture2d")
		{
			materialProperty.m_type = MaterialProperty::Type::TEXTURE2D;
			nlohmann::json::string_t value = propsArray[i]["value"];
			materialProperty.m_valueName = value;
		}
		if (propType == "float")
		{
			materialProperty.m_type = MaterialProperty::Type::FLOAT;
			nlohmann::json::number_float_t value = propsArray[i]["value"];
			materialProperty.m_value = std::make_shared<float>(value);
		}
		if (propType == "int")
		{
			materialProperty.m_type = MaterialProperty::Type::INT;
			nlohmann::json::number_integer_t value = propsArray[i]["value"];
			materialProperty.m_value = std::make_shared<int>(value);
		}
		nlohmann::json::string_t propName = propsArray[i]["name"];
		materialProperty.m_name = propName;
		nlohmann::json::string_t shaderName = json["shader"];
		material->m_shaderName = shaderName;

		material->setProperty(materialProperty);
	}

	material->m_hasDependencies = true;
	material->m_loadStatus = AssetLoadStatus::LOADED;
}

void MaterialAsset::loadDependencies()
{
	for (auto& pair : m_properties)
	{
		if (pair.second.m_type == MaterialProperty::Type::TEXTURE2D)
		{
			MaterialProperty prop = m_properties.find(pair.first)->second;
			prop.m_value = std::dynamic_pointer_cast<void>(std::static_pointer_cast<TextureAsset>(AssetManager::getAsset(AssetType::TEXTURE2D, pair.second.m_valueName)));
			setProperty(prop);
		}
	}
	m_shader = std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(AssetType::SHADER, m_shaderName));
}
}