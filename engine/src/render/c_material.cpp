#include "pch.h"
#include "c_material.h"

namespace Quartz
{
	void Material::setProperty(const MaterialProperty& prop)
	{
		m_properties[prop.m_name] = prop;
	}

	void Material::removeProperty(const std::string& propertyName)
	{
		auto p = m_properties.find(propertyName);
		if (p == m_properties.end())
			return;

		m_properties.erase(p);
	}

	const MaterialProperty& Material::getProperty(const std::string& propertyName)
	{
		auto p = m_properties.find(propertyName);
		if (p == m_properties.end())
			return MaterialProperty();

		return p->second;
	}
}