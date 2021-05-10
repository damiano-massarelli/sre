#include "Engine.h"
#include "Test.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshLoader.h"

#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/mesh/MeshCreator.h"

#include <iostream>

DECLARE_TEST_SCENE("Mesh Creator", MeshCreatorTestScene)

void MeshCreatorTestScene::start() {
    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

    auto material = std::make_shared<PBRMaterial>();
    material->setAlbedoMap(Texture::loadFromFile("test_data/textures/uv.jpg"));

    auto cone = Engine::gameObjectManager.createGameObject(MeshCreator::cone(), material);
    auto coneNormals
        = Engine::gameObjectManager.createGameObject(MeshCreator::cone(.5f, 3), std::make_shared<PropMaterial>(true));
    coneNormals->transform.setParent(cone);
    cone->transform.setPosition(glm::vec3{ 5, 0, 0 });

    auto cube = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), material);
    cube->transform.setPosition(glm::vec3{ 2.5, 0, 0 });

    auto cylinder = Engine::gameObjectManager.createGameObject(MeshCreator::cylinder(0.5f, 10), material);
    auto cNorm = Engine::gameObjectManager.createGameObject(
        MeshCreator::cylinder(0.5f, 10), std::make_shared<PropMaterial>(true));
    cNorm->transform.setParent(cylinder);
    cylinder->transform.setPosition(glm::vec3{ -2.5, 0, 0 });

    auto sphere = Engine::gameObjectManager.createGameObject(MeshCreator::sphere(1.f, 20, 20), material);
    sphere->transform.setPosition(glm::vec3{ -5, 0, 0 });

    auto plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), material);
    plane->transform.setPosition(glm::vec3{ -7.5, 0, 0 });

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
}

void MeshCreatorTestScene::end() { }
