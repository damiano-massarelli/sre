#include "Engine.h"
#include "Test.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshLoader.h"

#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/Bloom.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/GodRays.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/PointLight.h"
#include "rendering/mesh/MeshCreator.h"

#include "rendering/effects/GammaCorrection.h"
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

DECLARE_TEST_SCENE("God Rays", GodRayTestScene)

void GodRayTestScene::start() {
    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>());
    Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());
    auto godRays = std::make_shared<GodRays>();
    godRays->setDensity(0.95f);
    godRays->setDecayRatio(0.95f);
    godRays->lightRadius = 0.3f;
    Engine::renderSys.effectManager.addEffect(godRays);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

    auto material = std::make_shared<PBRMaterial>();
    material->setAlbedo(glm::vec3{ 1.0f, 0.5f, 0.3f });

    auto parent = Engine::gameObjectManager.createGameObject();
    parent->addComponent(std::make_shared<MoveComponent>(parent));
    for (int i = 0; i < 10; i++) {
        auto child = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), material);
        child->transform.setParent(parent);
        child->transform.setLocalPosition(glm::vec3{ i - 4.5f, 0, 0 });
        child->transform.setScale(glm::vec3{ 0.1f, 2.5f, 0.1f });

        auto child2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), material);
        child2->transform.setParent(parent);
        child2->transform.setLocalPosition(glm::vec3{ 0, i - 4.5f, 0 });
        child2->transform.setScale(glm::vec3{ 2.5f, 0.1f, 0.1f });
    }

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<PointLight>(light));
    light->transform.setPosition(glm::vec3{ 0.0f, 2.0f, 12.0f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };
    light->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto directionalLight
        = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    directionalLight->name = "light";
    directionalLight->addComponent(std::make_shared<DirectionalLight>(light));
    directionalLight->transform.setPosition(glm::vec3{ -2.0f, 0.0f, -12.0f });
    Engine::renderSys.addLight(directionalLight);
    directionalLight->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };
    directionalLight->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    directionalLight->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    godRays->light = directionalLight;
}

void GodRayTestScene::end() { }
