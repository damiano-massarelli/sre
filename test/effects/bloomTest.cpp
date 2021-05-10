#include "rendering/effects/Bloom.h"
#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/FXAA.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/Light.h"
#include "rendering/light/PointLight.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"

#include <iostream>

DECLARE_TEST_SCENE("Bloom", BloomTestScene)

void BloomTestScene::start() {
    Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
    Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>());

    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.shadowMappingSettings.useFastShader = true;
    Engine::renderSys.shadowMappingSettings.width = 300;
    Engine::renderSys.shadowMappingSettings.height = 300;

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

    auto sponza = GameObjectLoader().fromFile("test_data/bloom/sponza.fbx");
    sponza->transform.setScale(glm::vec3{ 0.1f });

    auto light3 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light3->name = "light3";
    light3->addComponent(std::make_shared<DirectionalLight>(light3));
    light3->transform.setPosition(glm::vec3{ 0.0f, 100.0f, 0.0f });
    Engine::renderSys.addLight(light3);
    light3->getComponent<Light>()->diffuseColor = glm::vec3{ .5f, .3f, .2f };
    light3->getComponent<Light>()->specularColor = glm::vec3{ .5f, .3f, .2f };
    light3->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
    light3->transform.rotateBy(glm::angleAxis(glm::radians(90.0f), light3->transform.right()));

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<PointLight>(light));
    light->transform.setPosition(glm::vec3{ 0.0f, 3.0f, 0.0f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto light2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light2->name = "light2";
    light2->addComponent(std::make_shared<PointLight>(light2));
    Engine::renderSys.addLight(light2);
    light2->getComponent<Light>()->diffuseColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light2->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light2->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light3);
    gizmo->transform.setLocalRotation(glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f });
    gizmo->transform.setPosition(light3->transform.getPosition());
}

void BloomTestScene::end() { }
