#include "pch.h"
#include "asset/assets/c_shaderasset.h"
#include "asset/c_assetmanager.h"
#include "asset/filesystem/c_filestream.h"
#include "render/c_renderer.h"

namespace Quartz
{

ShaderAsset::ShaderAsset(const std::string& name, const std::string& absolutePath) :
	Asset(Asset::Type::SHADER, name, absolutePath)
{
	m_pipeline = Renderer::s_gengine.createPipeline();
}

void ShaderAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(ShaderAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
}

void ShaderAsset::setMatrixBuffer(const Transform& transform, unsigned int frameIndex)
{
	glm::mat4 projectionMatrix = glm::perspectiveFovLH(70.0f, (float)1920, (float)1080, 0.1f, 1000.0f);
	glm::mat4 viewMatrix = glm::identity<glm::mat4>();
	glm::mat4 modelMatrix = Transform::matrixFromTransform(transform, false);
	projectionMatrix = glm::transpose(projectionMatrix);
	modelMatrix = glm::transpose(modelMatrix);
	viewMatrix = glm::transpose(viewMatrix);
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	

	UINT bufferSize = sizeof(modelMatrix) + sizeof(viewMatrix) + sizeof(projectionMatrix) + sizeof(normalMatrix);
	void* buffer = new unsigned char[bufferSize];
	char* loc = (char*)buffer;

	memcpy(loc, &modelMatrix, sizeof(modelMatrix));
	loc += sizeof(modelMatrix);

	memcpy(loc, &viewMatrix, sizeof(viewMatrix));
	loc += sizeof(viewMatrix);

	memcpy(loc, &projectionMatrix, sizeof(projectionMatrix));
	loc += sizeof(projectionMatrix);

	memcpy(loc, &normalMatrix, sizeof(normalMatrix));
	loc += sizeof(normalMatrix);

	delete[] buffer;
}

void ShaderAsset::loadCallback(std::shared_ptr<void> callbackArgs)
{
	std::shared_ptr<ShaderAsset> shader = std::static_pointer_cast<ShaderAsset>(callbackArgs);

	FileStream stream;
	stream.loadFile(shader->m_absolutePath.c_str(), true);
	std::string rawJson = stream.readAllText();

	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(rawJson.c_str());
	}
	catch (const std::exception&)
	{
		QZERROR("Failed to parse JSON for \"{0}\"", shader->m_name);
		return;
	}

	nlohmann::json::string_t psSourceDir = json["ps_source"];
	nlohmann::json::string_t psEntryPoint = json["ps_entrypoint"];
	nlohmann::json::string_t vsSourceDir = json["vs_source"];
	nlohmann::json::string_t vsEntryPoint = json["vs_entrypoint"];

	nlohmann::json::array_t propsArray = json["props"];
	for (int i = 0; i < propsArray.size(); i++)
	{
		ShaderProperty shaderProperty = {};
		nlohmann::json::string_t propType = propsArray[i]["type"];
		if (propType == "texture2d")
		{
			shaderProperty.m_type = ShaderProperty::Type::TEXTURE2D;
		}
		if (propType == "samplerstate")
		{
			shaderProperty.m_type = ShaderProperty::Type::SAMPLERSTATE;
		}
		nlohmann::json::string_t name = propsArray[i]["name"];
		shaderProperty.m_name = name;
		shaderProperty.registerSlot = propsArray[i]["register"];


		shader->m_properties[name] = shaderProperty;
	}


	std::string vsPath = AssetManager::findFile(vsSourceDir);
	std::string psPath = AssetManager::findFile(psSourceDir);

	Pipeline::ShaderData shaderData;
	shaderData.psPath = &psPath;
	shaderData.psEntryPoint = &psEntryPoint;
	shaderData.vsPath = &vsPath;
	shaderData.vsEntryPoint = &vsEntryPoint;

	shader->m_pipeline = Renderer::s_gengine.createPipeline(shader->m_pipeline, shaderData);
	
	shader->m_loadStatus = Asset::Status::LOADED;
}
}