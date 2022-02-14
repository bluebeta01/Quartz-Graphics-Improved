#pragma once
#include <string>

namespace Quartz
{
struct ShaderProperty
{
	enum class Type
	{
		TEXTURE2D, SAMPLERSTATE
	};

	Type m_type;
	std::string m_name;
	int registerSlot = 0;
};
}