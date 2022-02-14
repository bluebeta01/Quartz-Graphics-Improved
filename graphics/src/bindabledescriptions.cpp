#include "bindabledescriptions.h"

ShaderCBufferDescription::ShaderCBufferDescription(const std::string& name, int slot) :
	m_name(name), m_slot(slot)
{

}

void ShaderCBufferDescription::addVariableDescription(const ShaderVariableDescription& description)
{
	m_variables.push_back(description);
}