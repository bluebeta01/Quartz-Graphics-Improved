#pragma once
#include <memory>
#include <list>
#include "device.h"
#include "resource.h"

enum class DescriptorHeapType
{
	CBV_SRV_UAV, DSV, RTV
};

struct DescriptorHeapCreateInfo
{
	std::shared_ptr<Device> device;
	int size;
	DescriptorHeapType type;
};

class DescriptorHeap : public Resource
{
public:
	DescriptorHeap(const DescriptorHeapCreateInfo& createInfo);

	int getNextFreeIndex();
	void freeIndex(int index);

	std::shared_ptr<Device> getDevice() const { return m_device; }
	int getSize() const { return m_size; }
	DescriptorHeapType getType() const { return m_type; }

private:
	std::shared_ptr<Device> m_device;
	int m_size;
	DescriptorHeapType m_type;
	std::list<int> m_freeIndices;
};