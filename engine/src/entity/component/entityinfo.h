#pragma once
#include <string>
#include "transform.h"
#include "entity\entity.h"
#include "entity/world.h"

namespace Quartz
{
struct EntityInfoComponent
{
	std::string name;
	Transform transform;

	EntityInfoComponent(const std::string& name, const Transform& transform) :
		name(name), transform(transform)
	{}

	Transform getWorldTransform() const
	{
		glm::vec3 parentPosition = { 0,0,0 };
		if (hasParent)
			parentPosition = World::getEntityRegister().get<EntityInfoComponent>(parent).getWorldTransform().position;

		Transform t = transform;
		t.position += parentPosition;
		return t;
	}

	void setParent(Entity parent)
	{
		this->parent = parent;
		hasParent = true;
	}

	void removeParent()
	{
		hasParent = false;
	}

private:
	Entity parent;
	bool hasParent = false;
};
}