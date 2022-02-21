#pragma once
#include <memory>
#include <vector>
#include <graphics.h>

namespace Quartz
{
class CBufferPool
{
public:
	CBufferPool(int poolSize, int bufferSize);
	void initialize();

	std::shared_ptr<CBuffer> getNextBuffer();
	std::shared_ptr<CBuffer> getBufferAtIndex(int index);

private:
	int m_poolSize = 0;
	int m_bufferSize = 0;
	std::vector<std::shared_ptr<CBuffer>> m_buffers;
	int m_nextIndex = 0;

	void growPool(int amount);
};
}