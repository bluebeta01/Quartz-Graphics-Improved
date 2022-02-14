#pragma once
#include <memory>
#include "device.h"

struct VBufferCreateInfo
{
	std::shared_ptr<Device> device;
	int size;
	int stride;
	int vertexCount;
};

class VBuffer : Resource
{
public:
	VBuffer(const VBufferCreateInfo& createInfo) :
		m_device(createInfo.device), m_size(createInfo.size), m_stride(createInfo.stride),
		m_vertexCount(createInfo.vertexCount)
	{}

	virtual void bufferData(void* data, int dataSize) = 0;

	std::shared_ptr<Device> getDevice() const { return m_device; }
	int getSize() const { return m_size; }
	int getStride() const { return m_stride; }
	int getVertexCount() const { return m_vertexCount; }

private:
	std::shared_ptr<Device> m_device;
	int m_size;
	int m_stride;
	int m_vertexCount;
};