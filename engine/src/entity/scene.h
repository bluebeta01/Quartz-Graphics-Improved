#pragma once
#include <vector>
#include "entity/entity.h"

namespace Quartz
{

class World;

class Scene
{
public:
	Scene() {}
	Scene(const std::string& name);

	const std::string& getName() const { return m_name; }
	const std::vector<Entity>& getEntities() const { return m_entities; }

private:
	std::string m_name;
	std::vector<Entity> m_entities;

	friend class World;
};
}