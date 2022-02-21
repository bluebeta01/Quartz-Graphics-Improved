#include "pch.h"
#include "entity/world.h"
#include "entity\component\entityinfo.h"

namespace Quartz
{
std::unordered_map<std::string, Scene> World::m_scenes;
entt::registry World::m_entityRegister;

void World::initialize()
{
	m_scenes["root_scene"] = Scene("root_scene");
}

Scene* World::getSceneByName(const std::string& name)
{
	auto& scene = m_scenes.find(name);
	if (scene != m_scenes.end())
		return &scene->second;
	return nullptr;
}

Scene* World::getRootScene()
{
	return getSceneByName("root_scene");
}

Entity World::createEntity(const std::string& name, const Transform& transform, Scene* scene)
{
	Entity entity = m_entityRegister.create();
	m_entityRegister.emplace<EntityInfoComponent>(entity, name, transform);
	scene->m_entities.push_back(entity);
	return entity;
}
}