#pragma once
#include <string>
#include <memory>
#include "device.h"
#include "bindabledescriptions.h"

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

	std::shared_ptr<Device> getDevice() const { return m_device; }
	const std::vector<ShaderCBufferDescription>& getCBufferDescriptions() const { return m_cBufferDescriptions; }
	const std::vector<ShaderBindableDescription>& getShaderBindableDescriptions() const { return m_shaderBindableDescriptions; }

private:
	std::vector<Device> m_device;
	std::vector<ShaderCBufferDescription> m_cBufferDescriptions;
	std::vector<ShaderBindableDescription> m_shaderBindableDescriptions;
};