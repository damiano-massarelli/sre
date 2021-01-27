#include "Engine.h"
#include "Test.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshLoader.h"

#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/Light.h"
#include "rendering/light/PointLight.h"
#include "rendering/mesh/MeshCreator.h"
#include "terrain/HeightMapTerrainHeightProvider.h"
#include "terrain/TerrainGenerator.h"

#include "rendering/effects/FXAA.h"
#include "rendering/effects/GammaCorrection.h"
#include "resourceManagment/RefCount.h"

#include <iostream>
#include <random>

DECLARE_TEST_SCENE("FXAA", FXAATestScene)

void FXAATestScene::start() {
    // Setup Effects
    Engine::renderSys.effectManager.enableEffects();

    const auto gammaCorrectionEffect = std::make_shared<GammaCorrection>();
    Engine::renderSys.effectManager.addEffect(gammaCorrectionEffect);

    const auto fxaaEffect = std::make_shared<FXAA>();

    // Setup Camera
    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });
    auto cameraComponent = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cameraComponent);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });
    Engine::renderSys.setCamera(camera);

    // Setup Scene
    auto uvMaterial = std::make_shared<PBRMaterial>();
    uvMaterial->setAlbedoMap(Texture::loadFromFile("test_data/textures/uv.jpg"));

    auto greyMaterial = std::make_shared<PBRMaterial>();
    greyMaterial->setAlbedo(glm::vec3{ .4F, .4F, .4F });

    auto cube = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), greyMaterial);
    cube->transform.setPosition(glm::vec3{ 0.F, 0.F, 0.F });
    cube->transform.scaleBy(glm::vec3{ 5.F });

    auto sphere = Engine::gameObjectManager.createGameObject(MeshCreator::sphere(1.f, 10, 10), greyMaterial);
    sphere->transform.setPosition(glm::vec3{ -15, 0, 0 });
    sphere->transform.scaleBy(glm::vec3{ 5.F });

    auto plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), uvMaterial);
    plane->transform.setPosition(glm::vec3{ 10, 0, 0 });
    plane->transform.scaleBy(glm::vec3{ 5.F });

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<DirectionalLight>(light));
    light->transform.setPosition(glm::vec3{ 0.0f, 0.0f, 15.0f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->ambientColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light);
    gizmo->transform.setPosition(light->transform.getPosition());
    light->transform.setRotation(glm::quat{ glm::vec3{ glm::radians(180.0f), 0.0f, 0.0f } });

    Engine::uiRenderer.addUIDrawer([fxaaEffect, gammaCorrectionEffect]() {
        EffectManager& effectManager = Engine::renderSys.effectManager;
        bool fxaaEnabled = effectManager.hasEffect(fxaaEffect);

        ImGui::Begin("FXAA Settings");

        if (ImGui::Checkbox("Enabled", &fxaaEnabled)) {
            fxaaEnabled ? effectManager.addEffectBefore(fxaaEffect, gammaCorrectionEffect) : effectManager.removeEffect(fxaaEffect);
        }

        ImGui::End();
    });
}

void FXAATestScene::end() { }
