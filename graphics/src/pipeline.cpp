#include "pipeline.h"

std::shared_ptr<ShaderBindableDescription> Pipeline::findBindableDescriptionByName(std::string& name) const
{
	for (auto& bindable : m_bindables)
		if (bindable->name == name)
			return bindable;
}
