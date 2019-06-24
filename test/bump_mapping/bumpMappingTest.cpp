#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "PropMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"

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
#ifdef bumpMapping
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);
	//Engine::renderSys.shadowMappingSettings.setShadowStrength(0.0f);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});

    auto bumped = GameObjectLoader().fromFile("test_data/bump_mapping/bumped.fbx");
    bumped->addComponent(std::make_shared<MoveComponent>(bumped));
	bumped->transform.setPosition(glm::vec3{ 6, 0, -12 });
	bumped->transform.scaleBy(glm::vec3{ 0.3f });

	auto notBumped = GameObjectLoader().fromFile("test_data/bump_mapping/not_bumped.fbx");
	notBumped->addComponent(std::make_shared<MoveComponent>(notBumped));
	notBumped->transform.setPosition(glm::vec3{ -6, 0, -12 });
	notBumped->transform.scaleBy(glm::vec3{ 0.3f });

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
