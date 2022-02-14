#pragma once
#include <memory>
#include <vector>
#include "device.h"
#include "descheap.h"
#include "bindabledescriptions.h"
#include "resource.h"

struct CBufferCreateInfo
{
	std::shared_ptr<Device> device;
	std::shared_ptr<DescriptorHeap> descriptorHeap;
	std::shared_ptr<ShaderCBufferDescription> bufferDescription;
	int size;
};

class CBuffer : public Resource
{
public:
	CBuffer(const CBufferCreateInfo& createInfo) :
		m_device(createInfo.device), m_descriptorHeap(createInfo.descriptorHeap), m_size(createInfo.size),
		m_bufferDescription(createInfo.bufferDescription)
	{}

	virtual void bufferData(void* data, int dataSize) = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	std::shared_ptr<DescriptorHeap> getDescriptorHeap() const { return m_descriptorHeap; }
	int getSize() const { return m_size; }
private:
	std::shared_ptr<Device> m_device;
	std::shared_ptr<DescriptorHeap> m_descriptorHeap;
	int m_size;
	std::shared_ptr<ShaderCBufferDescription> m_bufferDescription;
};