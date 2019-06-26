#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "PropMaterial.h"

#include "FreeCameraComponent.h"
#include "Light.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"
#include "FXAA.h"
#include <memory>
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
#ifdef parallaxMapping
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

	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());

    auto withParallax = GameObjectLoader().fromFile("test_data/parallax_mapping/parallax.fbx");
	auto material = std::dynamic_pointer_cast<BlinnPhongMaterial>(withParallax->transform.getChildren()[0]->getMaterials()[0]);
	if (material)
		material->shininess = 72.0f;
    withParallax->addComponent(std::make_shared<MoveComponent>(withParallax));
	withParallax->transform.setPosition(glm::vec3{ 6, 0, -12 });
	withParallax->transform.scaleBy(glm::vec3{ 0.3f });

	auto noParallax = GameObjectLoader().fromFile("test_data/parallax_mapping/no_parallax.fbx");
	material = std::dynamic_pointer_cast<BlinnPhongMaterial>(noParallax->transform.getChildren()[0]->getMaterials()[0]);
	if (material)
		material->shininess = 72.0f;
	noParallax->addComponent(std::make_shared<MoveComponent>(noParallax));
	noParallax->transform.setPosition(glm::vec3{ -6, 0, -12 });
	noParallax->transform.scaleBy(glm::vec3{ 0.3f });

    auto light3 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light3->name = "light3";
    light3->addComponent(std::make_shared<Light>(light3, Light::Type::POINT));
    light3->transform.setPosition(glm::vec3{0.0f, 0.0f, 5.0f});
    Engine::renderSys.addLight(light3);
	light3->getComponent<Light>()->attenuationLinear = 0.01f;
	light3->getComponent<Light>()->attenuationQuadratic = 0;
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
