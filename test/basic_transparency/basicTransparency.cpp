#include "Engine.h"
#include "MeshLoader.h"
#include "PhongMaterial.h"
#include "LightMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"

#include <runTest.h>

#include <iostream>


#ifdef basicTransparency
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);

    auto camera = Engine::renderSys.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});

    auto surfaces = GameObjectLoader().fromFile("test_data/basic_transparency/suzanne.fbx");


    auto light = Engine::renderSys.createGameObject(MeshCreator::cube(), std::make_shared<LightMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<Light>(light));
    light->transform.setPosition(glm::vec3{0.0f, 3.0f, 0.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto light2 = Engine::renderSys.createGameObject(MeshCreator::cube(), std::make_shared<LightMaterial>());
    light2->name = "light2";
    light2->addComponent(std::make_shared<Light>(light2));
    Engine::renderSys.addLight(light2);
    light2->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light2->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};

    light2->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto light3 = Engine::renderSys.createGameObject(MeshCreator::cube(), std::make_shared<LightMaterial>());
    light3->name = "light3";
    light3->addComponent(std::make_shared<Light>(light3, Light::Type::DIRECTIONAL));
    light3->transform.setPosition(glm::vec3{0.0f, 0.0f, 15.0f});
    Engine::renderSys.addLight(light3);
    light3->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light3->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light3->getComponent<Light>()->innerAngle = glm::radians(25.0f);
    light3->getComponent<Light>()->outerAngle = glm::radians(28.0f);
    light3->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});


    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light3);
    gizmo->transform.setPosition(light3->transform.getPosition());
    light3->transform.setRotation(glm::quat{glm::vec3{glm::radians(180.0f), 0.0f, 0.0f}});

    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
