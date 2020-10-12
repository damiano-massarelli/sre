#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"

#include <iostream>

struct MoveComponent : public Component, public EventListener {

    CrumbPtr mEnterFrameCrumb;

    MoveComponent(const GameObjectEH& eh)
        : Component{ eh } {
        mEnterFrameCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
    }

    virtual void onEvent(SDL_Event e) override {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_RIGHT])
            gameObject->transform.moveBy(glm::vec3{ 0.1f, 0.0f, 0.0f });
        if (keys[SDL_SCANCODE_UP]) {
            gameObject->transform.rotateBy(glm::quat{ glm::vec3{ 0.0f, 0.15f, 0.0f } });
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            gameObject->transform.scaleBy(glm::vec3{ 1.0015f, 1.0015f, 1.0015f });
        }
    }
};

DECLARE_TEST_SCENE("Transform Local", TransformLocalTestScene)

void TransformLocalTestScene::start() {
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
    material->setAlbedo(glm::vec3{ 1.f, 0.5f, 0.3f });

    auto parent = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), material);
    parent->addComponent(std::make_shared<MoveComponent>(parent));
    auto child = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), material);
    child->transform.setParent(parent);
    child->transform.setLocalPosition(glm::vec3{ 2, 0, 0 });
    child->transform.setLocalRotation(glm::quat{ glm::vec3{ 0.0f, glm::radians(30.0f), 0.0f } });

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<PointLight>(light));
    light->transform.setPosition(glm::vec3{ 0.0f, 3.0f, 0.0f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto light2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light2->name = "light2";
    light2->addComponent(std::make_shared<PointLight>(light2));
    Engine::renderSys.addLight(light2);
    light2->getComponent<Light>()->diffuseColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light2->transform.moveBy(glm::vec3{ 0.f, 1.f, 5.f });
    light2->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
}

void TransformLocalTestScene::end() { }
