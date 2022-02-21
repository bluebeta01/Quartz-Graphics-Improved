#include "pch.h"
#include "editor.h"

namespace Quartz::Editor
{
Engine Editor::m_engine;
Entity boxParent;
Entity box;
void Editor::initialize()
{
	m_engine.initialize(true);

	boxParent = World::createEntity("boxparent", Transform(glm::vec3(0, 0, -10)), World::getRootScene());
	
	box = World::createEntity("box", Transform(glm::vec3(0, 0, 0)), World::getRootScene());
	EntityInfoComponent boxComp = World::getEntityRegister().get<EntityInfoComponent>(box);
	boxComp.setParent(boxParent);
	World::addComponentToEntity<EntityInfoComponent>(box, boxComp);

	RenderableComponent renderComp = {};
	renderComp.model = std::static_pointer_cast<ModelAsset>(AssetManager::getAsset(AssetType::MODEL, "models/blender/debug_object_1.qmf"));
	World::addComponentToEntity<RenderableComponent>(box, renderComp);

	while (!GameWindow::getTerminated())
		gameLoop();

	Renderer::deinitialize();
	AssetManager::deinitialize();
}

void Editor::gameLoop()
{
	m_engine.updateSystems();
	Renderer::beginRender();
	Renderer::setViewport(0, 0, 1280, 720);
	Renderer::setScissor(0, 0, 1280, 720);
	Renderer::clear();
	Renderer::renderWorld(glm::mat4(1.0f));
	Renderer::endRender();
}
}