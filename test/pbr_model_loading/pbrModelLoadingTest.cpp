#include "Engine.h"
#include "Test.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/BlinnPhongMaterial.h"
#include "rendering/materials/PropMaterial.h"

#include "cameras/FreeCameraComponent.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/Bloom.h"
#include "rendering/particle/ParticleEmitter.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/effects/MotionBlur.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/effects/SSAO.h"

#include <iostream>


DECLARE_TEST_SCENE("PBRModelLoading", PBRLoadingTestScene)

void PBRLoadingTestScene::start() {
	// Add effects
	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	Engine::renderSys.effectManager.addEffect(std::make_shared<MotionBlur>());
	auto gammaPost = std::make_shared<GammaCorrection>();
 	gammaPost->setGamma(2.2f);
 	gammaPost->setExposure(1.0f);
 	Engine::renderSys.effectManager.addEffect(gammaPost);

	// Create a camera
    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});								// set the camera position
	camera->transform.setRotation(glm::quat{ glm::vec3{0, glm::radians(180.0f), 0} });  // set camera rotation	 

	// FreeCameraComponent is a built-in component for an fps-like camera
    auto cam = std::make_shared<FreeCameraComponent>(camera);

	// Components can be added to to GameObjects
    camera->addComponent(cam);
    Engine::renderSys.setCamera(camera);

	auto helmet = GameObjectLoader{}.fromFile("test_data/damaged_helmet_pbr/DamagedHelmet.gltf");

	// Create an empy GameObject for the light
	auto sun = Engine::gameObjectManager.createGameObject();
	sun->name = "sun";

	// Lights are implemented as Components
	sun->addComponent(std::make_shared<DirectionalLight>(sun));
	sun->transform.setPosition(glm::vec3{ 50.0f, 205.0f, -65.0f });

	Engine::renderSys.addLight(sun);
	sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::NO_SHADOWS);
	sun->getComponent<Light>()->ambientColor = glm::vec3{ 0.1f } / 150.0f;
	sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f } * 5.0f;
	sun->transform.setPosition(glm::vec3{ 5.f, 5.f, 5.f });
	sun->transform.lookAt(glm::vec3{0.f});
}

void PBRLoadingTestScene::end() {}
