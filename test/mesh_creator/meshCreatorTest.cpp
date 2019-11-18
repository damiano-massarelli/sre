#include "Engine.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/BlinnPhongMaterial.h"
#include "rendering/materials/PropMaterial.h"

#include "cameras/FreeCameraComponent.h"
#include "rendering/light/Light.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"

#include "../test/runTest.h"

#include <iostream>


#ifdef meshCreator
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});


    MaterialPtr phong = BlinnPhongMaterialBuilder()
    .setDiffuseMap("test_data/mesh_creator/uv.jpg")
    .setShininess(32)
    .build();


    auto cone = Engine::gameObjectManager.createGameObject(MeshCreator::cone(), phong);
    auto coneNormals = Engine::gameObjectManager.createGameObject(MeshCreator::cone(), std::make_shared<PropMaterial>(true));
    coneNormals->transform.setParent(cone);
    cone->transform.setPosition(glm::vec3{5, 0, 0});

    auto cube = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), phong);
    cube->transform.setPosition(glm::vec3{2.5, 0, 0});

    auto cylinder = Engine::gameObjectManager.createGameObject(MeshCreator::cylinder(), phong);
    cylinder->transform.setPosition(glm::vec3{-2.5, 0, 0});

    auto sphere = Engine::gameObjectManager.createGameObject(MeshCreator::sphere(), phong);
    sphere->transform.setPosition(glm::vec3{-5, 0, 0});

	auto plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), phong);
	plane->transform.setPosition(glm::vec3{ -7.5, 0, 0 });

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<Light>(light));
    light->transform.setPosition(glm::vec3{0.0f, 3.0f, 0.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto light3 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
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
