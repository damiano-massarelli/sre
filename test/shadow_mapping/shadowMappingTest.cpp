#include "Engine.h"
#include "Test.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/BlinnPhongMaterial.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"

#include "cameras/FreeCameraComponent.h"
#include "rendering/light/Light.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"
#include "terrain/TerrainGenerator.h"
#include "terrain/HeightMapTerrainHeightProvider.h"
#include "resourceManagment/RefCount.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/effects/FXAA.h"
#include "rendering/materials/MultiTextureLambertMaterial.h"
#include "rendering/shadow/ShadowOnVisibleSceneComponent.h"

#include <stdlib.h>
#include <iostream>

DECLARE_TEST_SCENE("Shadow Mapping", ShadowMappingTestScene)

void ShadowMappingTestScene::start() {
	auto camera = Engine::gameObjectManager.createGameObject();
	camera->name = "camera";
	camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });

	auto cam = std::make_shared<FreeCameraComponent>(camera);
	camera->addComponent(cam);
	camera->transform.setRotation(glm::quat{ glm::vec3{0, glm::radians(180.0f), 0} });

    Engine::renderSys.setCamera(camera);

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


	auto multiTextured = std::make_shared<MultiTextureLambertMaterial>(
		Texture::loadFromFile("test_data/multiple_textures/grass.jpg"),
		Texture::loadFromFile("test_data/multiple_textures/ground.jpg"),
		Texture::loadFromFile("test_data/multiple_textures/path.jpg"),
		Texture::loadFromFile("test_data/multiple_textures/flowers.jpg"),
		Texture::loadFromFile("test_data/multiple_textures/blend.png"),
		40.0f,
		40.0f
		);

	for (int i = 0; i < 50; ++i) {
		auto go = GameObjectLoader().fromFile("test_data/shadow_mapping/tree.fbx");

		go->transform.setPosition(glm::vec3{rand() % 1024 - 512, 0, rand() % 1024 - 512 });
	}

	HeightMapTerrainHeightProvider hProvider{ "test_data/terrain/heightmap_2.png", 0, 0 };
	TerrainGenerator generator{ 512, 512, 1000, 1000 };
	auto terrain = Engine::gameObjectManager.createGameObject(generator.createTerrain(hProvider), multiTextured);

	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	//Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<DirectionalLight>(light));
    light->transform.setPosition(glm::vec3{-2.0f, 30.0f, -25.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});
	auto gizmo = MeshCreator::axisGizmo();
	gizmo->transform.setParent(light);
	gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });

	light->transform.rotateBy(glm::angleAxis(glm::radians(65.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }));
	light->transform.rotateBy(glm::angleAxis(glm::radians(15.0f), glm::vec3{ 0.0f, 1.0f, 0.0f }));
	
	light->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
	light->addComponent(std::make_shared<ShadowOnVisibleSceneComponent>(light));
}


void ShadowMappingTestScene::end() {}
