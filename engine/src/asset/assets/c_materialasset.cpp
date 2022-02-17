#include "pch.h"
#include "asset/assets/c_materialasset.h"
#include "asset/c_assetmanager.h"
#include "asset/filesystem/c_filestream.h"

namespace Quartz
{
MaterialAsset::MaterialAsset(const std::string& name, const std::string& abolsutePath) :
	Asset(Asset::Type::MATERIAL, name, abolsutePath), m_material(std::make_shared<Material>())
{

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

		material->m_material->setProperty(materialProperty);
	}

	material->m_shader = std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(Asset::Type::SHADER, shaderName));
	material->m_unloadedDependencies = true;
	material->m_loadStatus = Asset::Status::LOADED;
}

bool MaterialAsset::childrenLoaded()
{
	bool isLoaded = m_loadStatus == Asset::Status::LOADED;

	if (!isLoaded)
		return false;

	for (const std::pair<std::string, MaterialProperty>& pair : m_material->getPropertyMap())
	{
		if (pair.second.m_type == MaterialProperty::Type::TEXTURE2D)
		{
			std::shared_ptr<Asset> texture = AssetManager::getAsset(Asset::Type::TEXTURE2D, pair.second.m_valueName);
			if (texture != nullptr)
				isLoaded &= texture->childrenLoaded();
			else
				return false;
		}
	}

	std::shared_ptr<Asset> shader = AssetManager::getAsset(Asset::Type::SHADER, m_shaderName);
	if (shader != nullptr)
		isLoaded &= shader->childrenLoaded();
	else
		return false;

	//isLoaded &= m_material->m_pipeline != nullptr;
	//isLoaded &= m_shader->childrenLoaded();

	return isLoaded;
}

void MaterialAsset::loadDependencies()
{
	for (const std::pair<std::string, MaterialProperty>& pair : m_material->getPropertyMap())
	{
		if (pair.second.m_type == MaterialProperty::Type::TEXTURE2D)
		{
			MaterialProperty prop = m_material->getPropertyMap().find(pair.first)->second;
			prop.m_value = std::dynamic_pointer_cast<void>(std::static_pointer_cast<TextureAsset>(AssetManager::getAsset(Asset::Type::TEXTURE2D, pair.second.m_valueName)));
			m_material->setProperty(prop);
		}
	}
	m_material->setPipeline(std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(Asset::Type::SHADER, m_shaderName))->m_pipeline);
}
}