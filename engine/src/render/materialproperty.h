#pragma once
#include <string>
#include <memory>

namespace Quartz
{

struct MaterialProperty
{
	enum class Type
	{
		TEXTURE2D, INT, FLOAT
	};

	Type m_type = Type::FLOAT;
	std::string m_name;
	std::shared_ptr<void> m_value;
	std::string m_valueName;
};
}