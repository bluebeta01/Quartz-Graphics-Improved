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

	std::shared_ptr<ShaderCBufferDescription> findCBufferDescriptionByName(std::string& name) const;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	const std::unordered_map<int, std::shared_ptr<ShaderCBufferDescription>>& getCBufferDescriptions() const { return m_bufferDescriptions; }
	const std::vector<ShaderBindableDescription>& getShaderBindableDescriptions() const { return m_shaderBindableDescriptions; }

protected:
	std::shared_ptr<Device> m_device;
	std::unordered_map<int, std::shared_ptr<ShaderCBufferDescription>> m_bufferDescriptions;
	std::vector<ShaderBindableDescription> m_shaderBindableDescriptions;
};