#include "pch.h"
#include "asset/assets/c_shaderasset.h"
#include "asset/c_assetmanager.h"
#include "asset/filesystem/c_filestream.h"
#include "render/c_renderer.h"

namespace Quartz
{
ShaderAsset::ShaderAsset(const std::string& name, const std::string& absolutePath) :
	Asset(AssetType::SHADER, name, absolutePath)
{
	static int shaderId = 0;
	m_id = shaderId++;
}

bool ShaderAsset::dependenciesLoaded() const
{
	return m_loadStatus == AssetLoadStatus::LOADED;
}

void ShaderAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(ShaderAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
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

	std::string vsPath = AssetManager::findFile(vsSourceDir);
	std::string psPath = AssetManager::findFile(psSourceDir);

	nlohmann::json::boolean_t conservativeRaster = false;
	auto& conservative = json.find("conservative");
	if (conservative != json.end())
		conservativeRaster = *conservative;

	nlohmann::json::boolean_t depthTest = false;
	auto& depth = json.find("depth");
	if (depth != json.end())
		depthTest = *depth;

	nlohmann::json::boolean_t backfaceCulling = false;
	auto& culling = json.find("culling");
		if (culling != json.end())
			backfaceCulling = *culling;

	PipelineCreateInfo info = {};
	info.device = Renderer::s_device;
	info.id = shader->m_id;
	info.psEntryPoint = psEntryPoint;
	info.psPath = psPath;
	info.vsEntryPoint = vsEntryPoint;
	info.vsPath = vsPath;
	info.conservativeRaster = conservativeRaster;
	info.depthTest = depthTest;
	info.backfaceCulling = backfaceCulling;

	shader->m_pipeline = Pipeline::create(info);
	
	shader->m_loadStatus = AssetLoadStatus::LOADED;
}
void ShaderAsset::loadDependencies()
{
}
}