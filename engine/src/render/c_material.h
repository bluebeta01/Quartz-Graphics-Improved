#pragma once
#include <unordered_map>
#include <graphics.h>
#include "materialproperty.h"

namespace Quartz
{
	class Material
	{
	public:
		std::shared_ptr<Pipeline> m_pipeline;

		void setProperty(const MaterialProperty& prop);
		void removeProperty(const std::string& propertyName);

		const MaterialProperty& getProperty(const std::string& propertyName);
		constexpr const std::unordered_map<std::string, MaterialProperty>& getPropertyMap() { return m_properties; }
		void setPipeline(std::shared_ptr<Pipeline> pipeline) { m_pipeline = pipeline; }

	private:
		std::unordered_map<std::string, MaterialProperty> m_properties;
	};
}