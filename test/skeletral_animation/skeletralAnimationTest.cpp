#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "PropMaterial.h"
#include "SkyboxMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"
#include "TerrainGenerator.h"
#include "HeightMapTerrainHeightProvider.h"
#include "RefCount.h"
#include "GammaCorrection.h"
#include "FXAA.h"
#include "MultiTextureLambertMaterial.h"
#include "ShadowOnVisibleSceneComponent.h"
#include "SkeletalAnimationLoader.h"
#include "SkeletralAnimationControllerComponent.h"

#include "runTest.h"

#include <stdlib.h>
#include <iostream>

#ifdef skeletralAnimation  
int main(int argc, char* argv[]) {
	Engine::init();

	Engine::renderSys.createWindow(1280, 720);

	auto camera = Engine::gameObjectManager.createGameObject();
	camera->name = "camera";
	camera->transform.moveBy(glm::vec3{ 0.0f, 30.0f, 30.0f });
	camera->transform.setRotation(glm::quat{ glm::vec3{0, glm::radians(180.0f), 0} });
	Engine::renderSys.camera = camera;


	auto cam = std::make_shared<FreeCameraComponent>(camera);
	camera->addComponent(cam);
	
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

	HeightMapTerrainHeightProvider hProvider{ "test_data/terrain/heightmap_2.png", 0, 0 };
	TerrainGenerator generator{ 512, 512, 1000, 1000 };
	auto terrain = Engine::gameObjectManager.createGameObject(generator.createTerrain(hProvider), multiTextured);

	Engine::renderSys.shadowMappingSettings.useFastShader = false;
	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	//Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());

	auto human = GameObjectLoader().fromFile("test_data/skeletral_animation/human.dae");
	auto animationController = human->getComponent<SkeletralAnimationControllerComponent>();
	auto animation2 = SkeletalAnimationLoader().fromFile("test_data/skeletral_animation/human_animation_2.dae", animationController->getBoneName2index());
	animation2.loopDirection = SkeletalAnimation::LoopDirection::BOUNCE;
	animationController->addAnimation("anim2", animation2);
	animationController->playAnimation("anim2");

	human->transform.scaleBy(glm::vec3{ 100.0f });

	camera->transform.lookAt(human->transform.getPosition());

	auto tree = GameObjectLoader().fromFile("test_data/shadow_mapping/tree.fbx");

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
	

	light->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::DYNAMIC);
	light->addComponent(std::make_shared<ShadowOnVisibleSceneComponent>(light));


    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
