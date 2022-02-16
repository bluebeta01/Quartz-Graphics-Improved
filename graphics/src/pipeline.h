#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "device.h"
#include "bindabledescriptions.h"
#include "cbuffer.h"

struct PipelineCreateInfo
{
	std::shared_ptr<Device> device;
	std::string psPath;
	std::string psEntryPoint;
	std::string vsPath;
	std::string vsEntryPoint;
	int id;
};

class Pipeline : Resource
{
public:
	Pipeline(const PipelineCreateInfo& createInfo) :
		m_device(createInfo.device), m_id(createInfo.id)
	{}

	static std::shared_ptr<Pipeline> create(const PipelineCreateInfo& createInfo);
	std::shared_ptr<ShaderBindableDescription> findBindableDescriptionByName(std::string& name) const;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	const std::vector<std::shared_ptr<ShaderBindableDescription>>& getShaderBindableDescriptions() const { return m_bindables; }
	int getId() const { return m_id; }
	int getCBufferCount() const { return m_cBufferCount; }
	int getTextureCount() const { return m_textureCount; }

protected:
	std::shared_ptr<Device> m_device;
	std::vector<std::shared_ptr<ShaderBindableDescription>> m_bindables;
	int m_id = 0;
	int m_cBufferCount = 0;
	int m_textureCount = 0;
};