#include "pch.h"
#include "asset/assets/c_modelasset.h"
#include "asset/filesystem/c_filestream.h"
#include "asset/c_assetmanager.h"
#include "render/c_renderer.h"

namespace Quartz
{
void ModelAsset::load(std::shared_ptr<Asset> asset)
{
	AssetManager::s_threadPool.submitJob(ModelAsset::loadCallback, std::static_pointer_cast<void>(asset), nullptr);
}

void ModelAsset::loadCallback(std::shared_ptr<void> callbackArguments)
{
	std::shared_ptr<ModelAsset> model = std::static_pointer_cast<ModelAsset>(callbackArguments);

	FileStream stream;
	stream.loadFile(model->m_absolutePath.c_str(), false);

	char magic[16];
	stream.readBytes(16, magic);
	if (strncmp(magic, "QUARTZMODELFILE", 15))
	{
		QZERROR("Failed to load invalid QMF file \"{0}\"", model->m_name);
		return;
	}

	int floatCountPerVertex = 0;

	model->m_rawData = std::make_shared<std::vector<float>>();
	stream.readInt(&model->m_fileVersion);
	model->m_modelName = stream.readString();
	model->m_materialName = stream.readString();
	stream.readInt(&model->m_vertexCount);
	stream.readInt(&floatCountPerVertex);
	model->m_vertexSize = floatCountPerVertex * sizeof(float);

	for (int i = 0; i < model->m_vertexCount * floatCountPerVertex; i++)
	{
		float currentFloat;
		stream.readBytes(sizeof(float), &currentFloat);
		model->m_rawData->push_back(currentFloat);
	}

	UINT vertexBufferSize = model->m_vertexCount * model->m_vertexSize;
	
	VBufferCreateInfo info = {};
	info.device = Renderer::s_device;
	info.size = vertexBufferSize;
	info.stride = floatCountPerVertex * sizeof(float);
	info.vertexCount = model->m_vertexCount;
	model->m_vBuffer = VBuffer::create(info);

	model->m_hasDependencies = true;
	model->m_loadStatus = AssetLoadStatus::READY_FOR_GPU_UPLOAD;
}
void ModelAsset::clearRawData()
{
	m_rawData = nullptr;
}
bool ModelAsset::dependenciesLoaded() const
{
	if (m_material == nullptr)
		return false;
	return m_material->dependenciesLoaded() & m_loadStatus == AssetLoadStatus::LOADED;
}

void ModelAsset::loadDependencies()
{
	m_material = std::dynamic_pointer_cast<MaterialAsset>(AssetManager::getAsset(AssetType::MATERIAL, m_materialName));
}
}