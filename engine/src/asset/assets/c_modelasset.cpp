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

	model->m_modelData.rawModelData = std::make_shared<std::vector<float>>();
	stream.readInt(&model->m_modelData.qmfVersion);
	model->m_modelData.modelName = stream.readString();
	model->m_modelData.materialName = stream.readString();
	stream.readInt(&model->m_modelData.vertCount);
	stream.readInt(&model->m_modelData.floatsPerVert);

	int floatCount = model->m_modelData.vertCount * model->m_modelData.floatsPerVert;

	for (int i = 0; i < floatCount; i++)
	{
		float currentFloat;
		stream.readBytes(sizeof(float), &currentFloat);
		model->m_modelData.rawModelData->push_back(currentFloat);
	}

	UINT vertexBufferSize = model->m_modelData.vertCount * model->m_modelData.floatsPerVert * sizeof(float);
	//model->m_vBuffer = Renderer::s_gengine.createVBuffer(vertexBufferSize, sizeof(float) * model->m_modelData.floatsPerVert, model->m_modelData.vertCount);
	
	model->m_unloadedDependencies = true;
	model->m_loadStatus = Asset::Status::READY_FOR_GPU_UPLOAD;
}
bool ModelAsset::childrenLoaded()
{
	if (m_loadStatus != Asset::Status::LOADED || m_material == nullptr)
		return false;
	return m_material->childrenLoaded();
}

void ModelAsset::loadDependencies()
{
	m_material = std::dynamic_pointer_cast<MaterialAsset>(AssetManager::getAsset(Asset::Type::MATERIAL, m_modelData.materialName));
}
}