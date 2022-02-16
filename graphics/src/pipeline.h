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
};

class Pipeline : Resource
{
public:
	Pipeline(const PipelineCreateInfo& createInfo) :
		m_device(createInfo.device)
	{}

	std::shared_ptr<ShaderBindableDescription> findBindableDescriptionByName(std::string& name) const;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	const std::vector<std::shared_ptr<ShaderBindableDescription>>& getShaderBindableDescriptions() const { return m_bindables; }

protected:
	std::shared_ptr<Device> m_device;
	std::vector<std::shared_ptr<ShaderBindableDescription>> m_bindables;
};