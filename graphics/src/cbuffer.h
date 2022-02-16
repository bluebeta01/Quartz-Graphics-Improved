#pragma once
#include <memory>
#include <vector>
#include "device.h"
#include "bindabledescriptions.h"
#include "graphicsresource.h"

struct CBufferCreateInfo
{
	std::shared_ptr<Device> device;
	int size;
};

class CBuffer : public Resource
{
public:
	CBuffer(const CBufferCreateInfo& createInfo) :
		m_device(createInfo.device), m_size(createInfo.size)
	{}

	static std::shared_ptr<CBuffer> create(const CBufferCreateInfo& createInfo);
	virtual void bufferData(void* data, int dataSize) = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	int getSize() const { return m_size; }
protected:
	std::shared_ptr<Device> m_device;
	int m_size;
};