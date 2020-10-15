#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/Bloom.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/effects/MotionBlur.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/particle/ParticleEmitter.h"

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
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 10.0f });                             // set the camera position
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });  // set camera rotation

    // FreeCameraComponent is a built-in component for an fps-like camera
    auto cam = std::make_shared<FreeCameraComponent>(camera);
    cam->setCameraSensitivity(0.2f);

    // Components can be added to to GameObjects
    camera->addComponent(cam);
    Engine::renderSys.setCamera(camera);

    auto helmet = GameObjectLoader{}.fromFile("test_data/damaged_helmet_pbr/DamagedHelmet.gltf");
    PBRMaterial* pbrMaterial = static_cast<PBRMaterial*>(helmet->getMaterials()[0].get());

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

    // Create an empy GameObject for the light
    auto sun = Engine::gameObjectManager.createGameObject();
    sun->name = "sun";

    // Lights are implemented as Components
    sun->addComponent(std::make_shared<DirectionalLight>(sun));
    Engine::renderSys.addLight(sun);
    sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::NO_SHADOWS);
    sun->getComponent<Light>()->ambientColor = glm::vec3{ 0.003f };
    sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f } * 50.0f;
    sun->transform.setPosition(glm::vec3{ 0.f, 5.f, 1.f });
    sun->transform.lookAt(glm::vec3{ 0.f });
}

void PBRLoadingTestScene::end() { }
