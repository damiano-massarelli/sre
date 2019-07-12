#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "PropMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"
#include "FXAA.h"
#include "Bloom.h"
#include <runTest.h>

#include <iostream>


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

#ifdef bloomTest
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>());


	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.shadowMappingSettings.useFastShader = true;
	Engine::renderSys.shadowMappingSettings.width = 300;
	Engine::renderSys.shadowMappingSettings.height = 300;

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});

	auto sponza = GameObjectLoader().fromFile("test_data/bloom/sponza.fbx");
	sponza->transform.setScale(glm::vec3{ 0.1f });

	auto light3 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
	light3->name = "light3";
	light3->addComponent(std::make_shared<Light>(light3, Light::Type::DIRECTIONAL));
	light3->transform.setPosition(glm::vec3{ 0.0f, 100.0f, 0.0f });
	Engine::renderSys.addLight(light3);
	light3->getComponent<Light>()->castShadow = true;
	light3->getComponent<Light>()->diffuseColor = glm::vec3{ .5f, .3f, .2f };
	light3->getComponent<Light>()->specularColor = glm::vec3{ .5f, .3f, .2f };
	light3->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
	light3->transform.rotateBy(glm::angleAxis(glm::radians(90.0f), light3->transform.right()));

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<Light>(light));
    light->transform.setPosition(glm::vec3{0.0f, 3.0f, 0.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto light2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light2->name = "light2";
    light2->addComponent(std::make_shared<Light>(light2));
    Engine::renderSys.addLight(light2);
    light2->getComponent<Light>()->diffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light2->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light2->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light3);
	gizmo->transform.setLocalRotation(glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f });
    gizmo->transform.setPosition(light3->transform.getPosition());

    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
