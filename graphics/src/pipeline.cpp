#include "pipeline.h"

std::shared_ptr<ShaderCBufferDescription> Pipeline::findCBufferDescriptionByName(std::string& name) const
{
	for (auto& pair : m_bufferDescriptions)
		if (pair.second->getName() == name)
			return pair.second;
}
