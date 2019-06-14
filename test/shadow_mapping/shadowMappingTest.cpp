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
#include "GammaCorrection.h"
#include "FXAA.h"
#include "MultiTextureLambertMaterial.h"
#include "ShadowOnVisibleSceneComponent.h"

#include <runTest.h>

#include <stdlib.h>
#include <iostream>

#ifdef shadowMapping  
int main(int argc, char* argv[]) {
	Engine::init();

	Engine::renderSys.createWindow(1280, 720);

	auto camera = Engine::gameObjectManager.createGameObject();
	camera->name = "camera";
	camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });
	Engine::renderSys.camera = camera;

	auto cam = std::make_shared<FreeCameraComponent>(camera);
	camera->addComponent(cam);
	camera->transform.setRotation(glm::quat{ glm::vec3{0, glm::radians(180.0f), 0} });

	auto skyTexture = Texture::loadCubamapFromFile({
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
		auto go = GameObjectLoader().fromFile("test_data/shadow_mapping/cubes.fbx");
		go->transform.setPosition(glm::vec3{rand() % 1024 - 512, 0, rand() % 1024 - 512 });
	}

	HeightMapTerrainHeightProvider hProvider{ "test_data/terrain/heightmap_2.png", 0, 0 };
	TerrainGenerator generator{ 512, 512, 1000, 1000 };
	//Engine::gameObjectManager.createGameObject(generator.createTerrain(hProvider), std::make_shared<PropMaterial>(true));
	auto terrain = Engine::gameObjectManager.createGameObject(generator.createTerrain(hProvider), multiTextured);

	//Engine::renderSys.effectManager.enableEffects();
	//Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	//Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());


    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<Light>(light, Light::Type::DIRECTIONAL));
    light->transform.setPosition(glm::vec3{-2.0f, 30.0f, -25.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

	auto gizmo = MeshCreator::axisGizmo();
	gizmo->transform.setParent(light);
	gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });

	light->transform.rotateBy(glm::angleAxis(glm::radians(90.0f), glm::vec3{ 1, 0, 0 }));
	light->getComponent<Light>()->castShadow = true;
	light->addComponent(std::make_shared<ShadowOnVisibleSceneComponent>(light));

//     auto light2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
//     light2->name = "light2";
//     light2->addComponent(std::make_shared<Light>(light2));
//     Engine::renderSys.addLight(light2);
//     light2->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
//     light2->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
// 
//     light2->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});
// 
//     auto light3 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
//     light3->name = "light3";
//     light3->addComponent(std::make_shared<Light>(light3, Light::Type::DIRECTIONAL));
//     light3->transform.setPosition(glm::vec3{0.0f, 0.0f, 15.0f});
//     //Engine::renderSys.addLight(light3);
//     light3->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
//     light3->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
//     light3->getComponent<Light>()->innerAngle = glm::radians(25.0f);
//     light3->getComponent<Light>()->outerAngle = glm::radians(28.0f);
//     light3->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});
// 
// 
//     auto gizmo = MeshCreator::axisGizmo();
//     gizmo->transform.setParent(light3);
//     gizmo->transform.setPosition(light3->transform.getPosition());
//     light3->transform.setRotation(glm::quat{glm::vec3{glm::radians(90.0f), 0.0f, 0.0f}});

    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
