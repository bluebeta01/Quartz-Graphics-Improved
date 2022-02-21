#pragma once
#include <unordered_map>
#include <string>
#include <entt/entt.hpp>
#include "entity/scene.h"
#include "transform.h"

namespace Quartz
{
class World
{
public:
	static void initialize();

	static Scene* getSceneByName (const std::string& name);
	static Scene* getRootScene();
	static Entity createEntity(const std::string& name, const Transform& transform, Scene* scene);

	template <class ComponentType>
	static void addComponentToEntity(Entity entity, ComponentType component)
	{
		m_entityRegister.emplace_or_replace<ComponentType>(entity, component);
	};
	


	static const std::unordered_map<std::string, Scene>& getScenes() { return m_scenes; }
	static const entt::registry& getEntityRegister() { return m_entityRegister; }

private:
	World() {}

	static std::unordered_map<std::string, Scene> m_scenes;
	static entt::registry m_entityRegister;
};
}