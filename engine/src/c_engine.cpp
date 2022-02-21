#include "pch.h"
#include "c_engine.h"
#include "window/c_gamewindow.h"
#include "input/c_input.h"
#include "asset/c_assetmanager.h"
#include "asset/assets/c_textasset.h"
#include "asset/assets/c_modelasset.h"
#include "asset/filesystem/c_filestream.h"
#include "render/c_renderer.h"
#include "c_threadpool.h"
#include "entity\world.h"
#include "entity\component\renderable.h"
#include "entity/component/entityinfo.h"

namespace Quartz
{
Engine::Engine() : m_editorMode(false)
{

}

void Engine::initialize(bool editorMode, HWND windowHandle)
{
	m_editorMode = editorMode;
	initializeSystems(windowHandle);
	if (!editorMode)
		initGameLoop();
}
Entity box;
void Engine::initGameLoop()
{
	box = World::createEntity("box", Transform(glm::vec3(0,0,-10)), World::getRootScene());
	RenderableComponent renderComp = {};
	renderComp.model = std::static_pointer_cast<ModelAsset>(AssetManager::getAsset(AssetType::MODEL, "models/blender/debug_object_1.qmf"));
	World::addComponentToEntity<RenderableComponent>(box, renderComp);

	while (!GameWindow::getTerminated())
		gameLoopBody();

	Renderer::deinitialize();
	AssetManager::deinitialize();
}

void Engine::gameLoopBody()
{
	static std::shared_ptr<ModelAsset> shiba = std::dynamic_pointer_cast<ModelAsset>(AssetManager::getAsset(AssetType::MODEL, "models/blender/debug_object_1.qmf"));

	static Transform ballTransform;
	ballTransform.position.z = -10;

	static float delta = 0;

	static long long oldFrameTime = 0;

	{
		long long frameTime1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		delta = (float)(frameTime1 - oldFrameTime) / 1000.0f;
		oldFrameTime = frameTime1;
		static float avg = 0;
		avg = avg + ((delta - avg) / 10);

		static float runningTime = 0;
		runningTime += delta;
		if (runningTime > 1)
		{
			runningTime = 0;
			QZINFO("Average FPS: {0}", 1 / avg);
		}

		//QZINFO("{0}", delta);
		//if (delta > 1.0f / 90.0f)
			//QZINFO("Frame drop: {0}ms, {1}fps", delta * 1000.0f, 1.0f / delta);

		updateSystems();

		if (GameWindow::getReiszedFlag())
		{
			Renderer::resize(GameWindow::getWidth(), GameWindow::getHeight());
		}

		if (Input::isMouseButtonDown(0))
		{
			EntityInfoComponent c = World::getEntityRegister().get<EntityInfoComponent>(box);
			c.transform.rotateY(3.0f * delta);
			World::addComponentToEntity<EntityInfoComponent>(box, c);
		}

		if (Input::isKeyDownThisFrame('U'))
		{
			std::shared_ptr<ShaderAsset> shader = std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(AssetType::SHADER, "shaders/lightmap.qshader"));
			shiba->getMaterial()->setShader(shader);
		}
		if (Input::isKeyDownThisFrame('P'))
		{
			std::shared_ptr<ShaderAsset> shader = std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(AssetType::SHADER, "shaders/standard.qshader"));
			shiba->getMaterial()->setShader(shader);
		}
		if (Input::isKeyDownThisFrame('O'))
		{
			MaterialProperty matProp = {};
			matProp.m_name = "lightmap";
			matProp.m_type = MaterialProperty::Type::TEXTURE2D;
			matProp.m_value = std::static_pointer_cast<void>(AssetManager::getAsset(AssetType::TEXTURE2D, "textures/shiba_lightmap.png"));
			shiba->getMaterial()->setProperty(matProp);
			std::shared_ptr<ShaderAsset> shader = std::static_pointer_cast<ShaderAsset>(AssetManager::getAsset(AssetType::SHADER, "shaders/staticlit.qshader"));
			shiba->getMaterial()->setShader(shader);
		}

		if (Input::isKeyDownThisFrame('T'))
		{
			int lightmapSize = 256;

			Texture2DCreateInfo texInfo = {};
			texInfo.arraySize = 1;
			texInfo.device = Renderer::s_device;
			texInfo.height = lightmapSize;
			texInfo.width = lightmapSize;
			texInfo.type = TextureType::RENDER_TARGET;
			texInfo.textureResource = nullptr;
			std::shared_ptr<Texture2D> renderTexture = Texture2D::create(texInfo);

			texInfo.type = TextureType::DEPTH_STENCIL;
			std::shared_ptr<Texture2D> dsTexture = Texture2D::create(texInfo);

			std::vector<std::shared_ptr<Texture2D>> textures = { renderTexture, dsTexture };

			FramebufferCreateInfo fbInfo = {};
			fbInfo.device = Renderer::s_device;
			fbInfo.textureCount = 1;
			fbInfo.textures = &textures;
			std::shared_ptr<Framebuffer> fb = Framebuffer::create(fbInfo);

			std::shared_ptr<ShaderBindableDescription> cBufferBindableDesc =
				shiba->getMaterial()->getShader()->getPipeline()->findBindableDescriptionByName("matracies");

			Renderer::s_render3d->beginFrame(fb);
			Renderer::s_render3d->setViewport(0, 0, lightmapSize, lightmapSize);
			Renderer::s_render3d->setScissorRect(0, 0, lightmapSize, lightmapSize);
			Renderer::s_render3d->clearFrame();
			Renderer::renderEntity(box, glm::mat4(1.0f));
			Renderer::s_render3d->endFrame();
			Renderer::s_device->waitForIdle();

			

			std::vector<uint8_t> data = Renderer::s_device->readbackTexture(fb->getTextures()[0]);

			lodepng::encode("plugins/game/textures/shiba_lightmap.png", data, lightmapSize, lightmapSize);
		}


		Renderer::beginRender();
		Renderer::renderWorld(glm::mat4(1.0f));
		Renderer::endRender();
		
	}
}

void Engine::updateSystems()
{
	Input::update();
	GameWindow::update();
	AssetManager::update();
	Renderer::update();
}

void Engine::initializeSystems(HWND windowHandle)
{
	Log::initialize();
	Input::initialize();
	GameWindow::initialize(windowHandle);
	AssetManager::initialize();
	Renderer::initialize();
	World::initialize();
}
}