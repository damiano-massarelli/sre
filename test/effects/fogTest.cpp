#include "rendering/effects/Fog.h"
#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"

DECLARE_TEST_SCENE("Fog", FogTestScene)

void FogTestScene::start() {
    auto fogEffect = std::make_shared<Fog>();
    Engine::renderSys.effectManager.addEffect(fogEffect);
    Engine::renderSys.effectManager.enableEffects();

    // Initial fog settings
    fogEffect->setColor(glm::vec3{ 0.43f, 0.69f, 0.83f });  // light cyan
    fogEffect->setDensity(0.021f);
    fogEffect->setRapidity(5.f);

    // Camera and Sun (main light)
    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 10.0f, 0.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

    auto sun = Engine::gameObjectManager.createGameObject();
    sun->name = "sun";
    sun->addComponent(std::make_shared<DirectionalLight>(sun));
    sun->transform.setPosition(glm::vec3{ 0.0f, 5.0f, 0.0f });
    Engine::renderSys.addLight(sun);
    sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
    sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .7f, .7f } * 1.0f;
    sun->transform.rotateBy(glm::angleAxis(glm::radians(55.0f), sun->transform.right()));

    // Ground
    std::shared_ptr<PBRMaterial> groundMaterial = std::make_shared<PBRMaterial>();
    groundMaterial->setAlbedo(glm::vec3{ 0.1f, 0.35f, 0.1f });
    groundMaterial->setRoughness(0.95f);
    groundMaterial->setMetalness(0.f);

    GameObjectEH plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), groundMaterial);
    plane->transform.setRotation(glm::angleAxis(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)));
    plane->transform.scaleBy(glm::vec3(1000.f));

    // Spheres
    std::shared_ptr<PBRMaterial> redMaterial = std::make_shared<PBRMaterial>();
    redMaterial->setAlbedo(glm::vec3{ 0.7, 0.1, 0.1 });

    constexpr float distanceBetweenSpheres = 10.f;

    for (int i = -10; i < 10; ++i) {
        for (int j = -10; j < 10; ++j) {
            GameObjectEH sphere = Engine::gameObjectManager.createGameObject(MeshCreator::sphere(), redMaterial);
            sphere->transform.setPosition(glm::vec3(i * distanceBetweenSpheres, 2.5f, j * distanceBetweenSpheres));
            sphere->transform.scaleBy(glm::vec3(5.f));
        }
    }

    Engine::uiRenderer.addUIDrawer([fogEffect, cam]() {
        ImGui::Begin("Settings");

        glm::vec3 fogColor = fogEffect->getColor();
        float fogStartDistance = fogEffect->getDensity();
        float fogRapidity = fogEffect->getRapidity();

        ImGui::ColorPicker3("Fog Color", &fogColor.x);
        ImGui::SliderFloat("Fog Density", &fogStartDistance, 0.0001f, 0.1f);
        ImGui::SliderFloat("Fog Rapidity", &fogRapidity, 1.f, 15.f);

        fogEffect->setColor(fogColor);
        fogEffect->setDensity(fogStartDistance);
        fogEffect->setRapidity(fogRapidity);

        ImGui::End();
    });
}

void FogTestScene::end() { }
