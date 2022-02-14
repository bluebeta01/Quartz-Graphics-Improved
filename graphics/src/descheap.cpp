#include "descheap.h"

DescriptorHeap::DescriptorHeap(const DescriptorHeapCreateInfo& createInfo) :
	m_device(createInfo.device), m_size(createInfo.size), m_type(createInfo.type)
{
	for (int i = 0; i < m_size; i++)
		m_freeIndices.push_back(i);
}

int DescriptorHeap::getNextFreeIndex()
{
	if (m_freeIndices.size() == 0)
	{
		//TODO: Log error here
		//QZERROR("Texture descriptor heap is out of available desciptors!");
	}
	int front = m_freeIndices.front();
	m_freeIndices.pop_front();
	return front;
}

void DescriptorHeap::freeIndex(int index)
{
	m_freeIndices.push_back(index);
}