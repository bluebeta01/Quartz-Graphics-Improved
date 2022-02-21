#include "pch.h"
#include "render/cbufferpool.h";
#include "render/c_renderer.h"

namespace Quartz
{
CBufferPool::CBufferPool(int poolSize, int bufferSize) :
	m_poolSize(poolSize), m_bufferSize(bufferSize)
{
	
}

void CBufferPool::initialize()
{
	growPool(m_poolSize);
}

std::shared_ptr<CBuffer> CBufferPool::getNextBuffer()
{
	int i = m_nextIndex;
	m_nextIndex = (m_nextIndex + 1) % m_poolSize;
	return m_buffers[i];
}

std::shared_ptr<CBuffer> CBufferPool::getBufferAtIndex(int index)
{
	return m_buffers[index];
}

void CBufferPool::growPool(int amount)
{
	CBufferCreateInfo info = {};
	info.device = Renderer::s_device;
	info.size = m_bufferSize;
	for (int i = 0; i < amount; i++)
		m_buffers.push_back(CBuffer::create(info));
}

}