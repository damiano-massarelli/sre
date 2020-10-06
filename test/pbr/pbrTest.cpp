#include "Engine.h"
#include "Test.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/PropMaterial.h"
#include "cameras/FreeCameraComponent.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/FXAA.h"
#include "rendering/particle/ParticleEmitter.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/effects/MotionBlur.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/effects/GammaCorrection.h"

#include <iostream>

DECLARE_TEST_SCENE("PBR", PBRTestScene)

void PBRTestScene::start() {
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
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 15.0f});								// set the camera position
	camera->transform.setRotation(glm::quat{ glm::vec3{0, glm::radians(180.0f), 0} });  // set camera rotation	 

	// FreeCameraComponent is a built-in component for an fps-like camera
    auto cam = std::make_shared<FreeCameraComponent>(camera);
	cam->setCameraSensitivity(0.2f);

	// Components can be added to to GameObjects
    camera->addComponent(cam);
    Engine::renderSys.setCamera(camera);

	// Create a PBR material
	auto pbrMaterial = std::make_shared<PBRMaterial>();
	pbrMaterial->setAlbedoMap(Texture::loadFromFile("test_data/pbr/albedo.png"));
	pbrMaterial->setAlbedo(glm::vec3{ 255, 118, 106 } / 255.0f);
	pbrMaterial->setMetalnessMap(Texture::loadFromFile("test_data/pbr/metalness.png"));
	pbrMaterial->setNormalMap(Texture::loadFromFile("test_data/pbr/normal.png"));
	pbrMaterial->setRoughnessMap(Texture::loadFromFile("test_data/pbr/roughness.png"));
	pbrMaterial->setAmbientOcclusionMap(Texture::loadFromFile("test_data/pbr/roughness.png"));

	Engine::uiRenderer.addUIDrawer([pbrMaterial, cam]() {
		float roughness = pbrMaterial->getRoughness();
		bool useRoughnessMap = pbrMaterial->getUseRoughnessMap();
		float metalness = pbrMaterial->getMetalness();
		bool useMetalnessMap = pbrMaterial->getUseMetalnessMap();
		float cameraSensitivity = cam->getCameraSensitivity();

	    ImGui::Begin("Settings");
		ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f);
		ImGui::Checkbox("Use Roughness Map", &useRoughnessMap);
		
		ImGui::SliderFloat("Metalness", &metalness, 0.f, 1.f);
		ImGui::Checkbox("Use Metalness Map", &useMetalnessMap);

		pbrMaterial->setRoughness(roughness);
		pbrMaterial->setUseRoughnessMap(useRoughnessMap);
		
		pbrMaterial->setMetalness(metalness);
		pbrMaterial->setUseMetalnessMap(useMetalnessMap);

		ImGui::SliderFloat("Sensitivity", &cameraSensitivity, 0.1f, 2.f, "%.1f");
		cam->setCameraSensitivity(cameraSensitivity);

	    ImGui::End();
	});

	pbrMaterial->setRoughness(0.1f);

	auto sphereGenerated = Engine::gameObjectManager.createGameObject(MeshCreator::sphere(0.5f, 50, 50), pbrMaterial);
	sphereGenerated->transform.setPosition(glm::vec3{ 0.f, 0.f, 0.f });
	sphereGenerated->transform.scaleBy(glm::vec3{ 3.0f });

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

void PBRTestScene::end() {}
