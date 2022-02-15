#include "bindabledescriptions.h"

ShaderCBufferDescription::ShaderCBufferDescription(const std::string& name, int slot, int id, int size) :
	m_name(name), m_slot(slot), m_id(id), m_size(size)
{

}

void ShaderCBufferDescription::addVariableDescription(const ShaderVariableDescription& description)
{
	m_variables.push_back(description);
}