#include "Engine.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/BlinnPhongMaterial.h"
#include "rendering/materials/PropMaterial.h"

#include "cameras/FreeCameraComponent.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/PointLight.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/GodRays.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/Bloom.h"
#include "../test/runTest.h"

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

#ifdef godRaysTest
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);

	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());

	auto godRays = std::make_shared<GodRays>();

	//Engine::renderSys.effectManager.addEffect(godRays);
	Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>());

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});


	MaterialPtr phong = BlinnPhongMaterialBuilder()
		.setDiffuseMap("test_data/transform_local/container.png")
		.setSpecularColor(glm::vec3{ 1, 1, 1 })
		//.setSpecularMap("test_data/transform_local/container_specular.png")
		.build();


    auto parent = Engine::gameObjectManager.createGameObject();
    parent->addComponent(std::make_shared<MoveComponent>(parent));
	for (int i = 0; i < 10; i++) {
		auto child = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), phong);
		child->transform.setParent(parent);
		child->transform.setLocalPosition(glm::vec3{i - 5, 0, 0});
		child->transform.setScale(glm::vec3{ 0.1f, 2.5f, 0.1f });

		auto child2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), phong);
		child2->transform.setParent(parent);
		child2->transform.setLocalPosition(glm::vec3{0, i - 5, 0 });
		child2->transform.setScale(glm::vec3{ 2.5f, 0.1f, 0.1f });
	}


    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<PointLight>(light));
    light->transform.setPosition(glm::vec3{0.0f, 0.0f, 12.0f});
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{10.0f, 10.0f, 10.0f};
    light->getComponent<Light>()->specularColor = glm::vec3{1.0f, 1.0f, 1.0f};
    light->transform.scaleBy(glm::vec3{0.2f, 0.2f, 0.2f});

	auto directionalLight = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
	directionalLight->name = "light";
	directionalLight->addComponent(std::make_shared<DirectionalLight>(light));
	directionalLight->transform.setPosition(glm::vec3{ -2.0f, 0.0f, -12.0f });
	Engine::renderSys.addLight(directionalLight);
	directionalLight->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };
	directionalLight->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
	directionalLight->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

	godRays->light = directionalLight;

    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
