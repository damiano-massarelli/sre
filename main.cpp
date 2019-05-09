#include "Engine.h"
#include "MeshLoader.h"
#include "PhongMaterial.h"
#include "LightMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "MeshCreator.h"

#include <iostream>

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

struct MoveComponent : public Component, public EventListener {

    MoveComponent(const GameObjectEH& eh) : Component{eh} {
        Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, false);
    }

    virtual void onEvent(SDL_Event e) override {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_RIGHT])
            gameObject->transform.moveBy(glm::vec3{0.1f, 0.0f, 0.0f});
        if (keys[SDL_SCANCODE_UP]) {
            gameObject->transform.rotateBy(glm::quat{glm::vec3{0.0f, 0.15f, 0.0f}});
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            gameObject->transform.scaleBy(glm::vec3{1.0015f, 1.0015f, 1.0015f});
        }
    }

};

struct DetachComponent : public Component, public EventListener {
    CrumbPtr crumb;

    DetachComponent(const GameObjectEH& eh) : Component{eh} {
        crumb = Engine::eventManager.addListenerFor(SDL_KEYDOWN, this, true);
    }

    virtual void onEvent(SDL_Event e) override {
        if (e.key.keysym.sym == SDLK_SPACE)
            Engine::renderSys.remove(gameObject);
    }

};

int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(800, 600);

    MaterialPtr phong = PhongMaterialBuilder()
        .setDiffuseMap("container.png")
        .setSpecularMap("container_specular.png")
        .build();
    auto go = Engine::renderSys.createGameObject(MeshLoader::createMesh(vertices, 36, nullptr, 0), phong);
    go->transform.setPosition(glm::vec3{0.0f, 0.0f, -3.0f});
    //go->transform.setRotation(glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f}));
    go->addComponent(std::make_shared<MoveComponent>(go));

    MaterialPtr phong2 = PhongMaterialBuilder()
        .setDiffuseMap("container.png")
        .setSpecularMap("container_specular.png")
        .build();
    auto go2 = Engine::renderSys.createGameObject(MeshLoader::createMesh(vertices, 36, nullptr, 0), phong2);
    go2->transform.setPosition(glm::vec3{10.0f, 0.0f, -3.0f});
    go2->addComponent(std::make_shared<DetachComponent>(go2));

    go->transform.addChild(go2);

    auto go3 = Engine::renderSys.createGameObject(MeshLoader::createMesh(vertices, 36, nullptr, 0), phong2);
    go3->transform.setPosition(glm::vec3{5.0f, 3.0f, -3.0f});
    //go2->transform.setRotation(glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f}));

    go3->transform.setParent(go2);

    auto camera = Engine::renderSys.createGameObject();
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});


    auto light = Engine::renderSys.createGameObject(MeshLoader::createMesh(vertices, 36, nullptr, 0), std::make_shared<LightMaterial>());
    light->addComponent(std::make_shared<Light>(light));
    light->transform.setPosition(glm::vec3{0.0f, 3.0f, 0.0f});
    //Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});
    //light->transform.setParent(go);

    auto light2 = Engine::renderSys.createGameObject(MeshLoader::createMesh(vertices, 36, nullptr, 0), std::make_shared<LightMaterial>());
    light2->addComponent(std::make_shared<Light>(light2));
    //Engine::renderSys.addLight(light2);
    light2->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light2->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};

    light2->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto light3 = Engine::renderSys.createGameObject(MeshLoader::createMesh(vertices, 36, nullptr, 0), std::make_shared<LightMaterial>());
    light3->addComponent(std::make_shared<Light>(light3, Light::Type::SPOT));
    light3->transform.setPosition(glm::vec3{0.0f, 10.0f, 0.0f});
    Engine::renderSys.addLight(light3);
    light3->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light3->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light3->getComponent<Light>()->innerAngle = glm::radians(15.0f);
    light3->getComponent<Light>()->outerAngle = glm::radians(18.0f);
    light3->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});


    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light3);
    gizmo->transform.setPosition(light3->transform.getPosition());

    light3->transform.setRotation(glm::quat{glm::vec3{glm::radians(90.0f), 0.0f, 0.0f}});
    //light2->transform.setParent(go);

    Engine::start();

    return 0;
}
