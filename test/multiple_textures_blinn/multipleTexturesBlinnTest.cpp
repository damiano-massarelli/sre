#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "PropMaterial.h"
#include "SkyboxMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"
#include "TerrainGenerator.h"
#include "HeightMapTerrainHeightProvider.h"
#include "RefCount.h"
#include "FXAA.h"
#include "MultiTextureBlinnPhongMaterial.h"
#include "ShadowOnVisibleSceneComponent.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "runTest.h"

#include <iostream>
#include <stdlib.h>

#ifdef multipleTextureBlinn 
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});

    auto skyTexture = Texture::loadCubemapFromFile({
                    {"front", "test_data/skybox/front.tga"},
                    {"back", "test_data/skybox/back.tga"},
                    {"top", "test_data/skybox/top.tga"},
                    {"bottom", "test_data/skybox/bottom.tga"},
                    {"left", "test_data/skybox/left.tga"},
                    {"right", "test_data/skybox/right.tga"},
                });
    auto skyboxMaterial = std::make_shared<SkyboxMaterial>(skyTexture);
    auto box = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), skyboxMaterial);

	auto multiTextured = std::make_shared<MultiTextureBlinnPhongMaterial>();
	multiTextured->baseTexture = Texture::loadFromFile("test_data/multiple_textures_blinn/grass.jpg");
	multiTextured->baseTextureBump = Texture::loadFromFile("test_data/multiple_textures_blinn/grass_bump.png");
	multiTextured->greenTexture = Texture::loadFromFile("test_data/multiple_textures_blinn/path.jpg");
	multiTextured->greenTextureSpecular = Texture::loadFromFile("test_data/multiple_textures_blinn/path.jpg");
	multiTextured->greenTextureBump = Texture::loadFromFile("test_data/multiple_textures_blinn/path_bump.jpg");
	multiTextured->redTexture = Texture::loadFromFile("test_data/multiple_textures_blinn/ground.jpg");
	multiTextured->redTextureSpecular = Texture::loadFromFile("test_data/multiple_textures_blinn/ground.jpg");
	multiTextured->redTextureBump = Texture::loadFromFile("test_data/multiple_textures_blinn/ground_bump.jpg");
	multiTextured->blendTexture = Texture::loadFromFile("test_data/multiple_textures_blinn/blend.png");
	multiTextured->greenShininess = 8.0f;

	HeightMapTerrainHeightProvider hProvider{ "test_data/multiple_textures_blinn/height.jpg", -5, 5 };
	TerrainGenerator generator{ 128, 128, 500, 500 };
	generator.includeTangentSpace(true);
	auto terrain = Engine::gameObjectManager.createGameObject(generator.createTerrain(hProvider), multiTextured);

	for (int i = 0; i < 50; i++) {
		auto tree = GameObjectLoader().fromFile("test_data/multiple_textures_blinn/trees.fbx");
		int x = (rand() % 500) - 250;
		int z = (rand() % 500) - 250;
		float y = hProvider.get((x + 250) / 500.0f, (z + 250) / 500.0f);
		tree->transform.setPosition(glm::vec3{ x, y - .5f, z });
	}

	for (int i = 0; i < 50; i++) {
		auto tree = GameObjectLoader().fromFile("test_data/multiple_textures_blinn/trees2.fbx");
		int x = (rand() % 500) - 250;
		int z = (rand() % 500) - 250;
		float y = hProvider.get((x + 250) / 500.0f, (z + 250) / 500.0f);
		tree->transform.setPosition(glm::vec3{ x, y - .5f, z });
	}

	for (int i = 0; i < 150; i++) {
		auto bush = GameObjectLoader().fromFile("test_data/multiple_textures_blinn/bush.fbx");
		int x = (rand() % 500) - 250;
		int z = (rand() % 500) - 250;
		float y = hProvider.get((x + 250) / 500.0f, (z + 250) / 500.0f);
		bush->transform.setPosition(glm::vec3{ x, y - .5f, z });
		bush->transform.scaleBy(glm::vec3{ 0.5f });
	}

	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	Engine::renderSys.shadowMappingSettings.setShadowStrength(0.3f);
	Engine::renderSys.shadowMappingSettings.useFastShader = false;

	auto light3 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
	light3->name = "light3";
	light3->addComponent(std::make_shared<DirectionalLight>(light3));
	light3->transform.setPosition(glm::vec3{ 0.0f, 0.0f, 15.0f });
	Engine::renderSys.addLight(light3);
	light3->getComponent<Light>()->diffuseColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
	light3->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::AUTO);
	light3->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
	light3->getComponent<Light>()->innerAngle = glm::radians(25.0f);
	light3->getComponent<Light>()->outerAngle = glm::radians(28.0f);
	light3->addComponent(std::make_shared<ShadowOnVisibleSceneComponent>(light3));
	light3->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<PointLight>(light));
    light->transform.setPosition(glm::vec3{0.0f, 3.0f, 0.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto light2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light2->name = "light2";
    light2->addComponent(std::make_shared<PointLight>(light2));
    Engine::renderSys.addLight(light2);
	light2->transform.setPosition(glm::vec3{ 3.0f, 3.0f, 0.0f });
    light2->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 0.0f, 0.0f};
    light2->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};

    light2->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});


    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light3);
	gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });
    light3->transform.setLocalRotation(glm::quat{glm::vec3{glm::radians(65.0f), 0.0f, 0.0f}});

    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
