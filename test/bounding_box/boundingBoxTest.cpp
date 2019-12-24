#include "Engine.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/BlinnPhongMaterial.h"
#include "rendering/materials/PropMaterial.h"
#include "debugUtils/DisplayBoundingBoxComponent.h"
#include "cameras/FreeCameraComponent.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/Bloom.h"
#include "../test/runTest.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/effects/MotionBlur.h"
#include "rendering/effects/GammaCorrection.h"
#include "debugUtils/DisplayCameraFrustumComponent.h"
#include "cameras/CameraComponent.h"

#include <imgui/imgui.h>

#include <iostream>

#ifdef boundingBox
struct MoveComponent : public Component, public EventListener {

	MoveComponent(const GameObjectEH& eh) : Component{ eh } {
		Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, false);
	}

	virtual void onEvent(SDL_Event e) override {
		float delta = (*(static_cast<float*>(e.user.data1))) / 1000.0f;
		const Uint8* keys = SDL_GetKeyboardState(nullptr);
		if (keys[SDL_SCANCODE_RIGHT])
			gameObject->transform.moveBy(glm::vec3{ 00.f, 10.f, 00.0f } * delta);
		if (keys[SDL_SCANCODE_UP]) {
			gameObject->transform.rotateBy(glm::quat{ glm::vec3{0.0f, 5.f, 0.0f} * delta });
		}
		if (keys[SDL_SCANCODE_DOWN]) {
			gameObject->transform.scaleBy(glm::vec3{ 1.0015f });
		}
	}
};


int main(int argc, char* argv[]) {
	Engine::init(); // init engine

	Engine::renderSys.createWindow(720, 720); // create a window

	// add effects
	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	Engine::renderSys.effectManager.addEffect(std::make_shared<MotionBlur>());
	auto gammaPost = std::make_shared<GammaCorrection>();
	gammaPost->setGamma(1.8f);
	gammaPost->setExposure(1.0f);
	Engine::renderSys.effectManager.addEffect(gammaPost);

		// create a camera
	auto camera = Engine::gameObjectManager.createGameObject();
	camera->name = "camera";
	camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });								// set the camera position
	camera->transform.setRotation(glm::quat{ glm::vec3{0, glm::radians(180.0f), 0} });  // set camera rotation

	// FreeCameraComponent is a built-in component for an fps-like camera
	auto cam = std::make_shared<FreeCameraComponent>(camera);

	// Components can be added to to GameObjects
	camera->addComponent(cam);

	Engine::renderSys.setCamera(camera);

	// Create a sphere and set its scale
	auto scene = GameObjectLoader{}.fromFile("test_data/bounding_box/spheres.fbx");

	auto cube = scene->transform.findAll("pCube1")[0];
	auto sphere = scene->transform.findAll("pSphere1")[0];
	auto torus = scene->transform.findAll("pTorus1")[0];

    Engine::uiRenderer.setDebugUIDrawer([cube]() {
        auto material = dynamic_cast<BlinnPhongMaterial*>((cube->getMaterials()[0]).get());
        glm::vec3 diffuse = material->diffuseColor;
        ImVec4 clear_color = ImVec4(diffuse.r, diffuse.g, diffuse.b, 1.00f);
        
        ImGui::Begin("Hello Window");
        ImGui::Text("This is a very useful text.");
        ImGui::ColorEdit3("Cube diffuse color: ", (float*)&clear_color);

        material->diffuseColor = glm::vec3{ clear_color.x, clear_color.y, clear_color.z };

        ImGui::End();
    });


	auto gizmo = MeshCreator::axisGizmo();
	gizmo->transform.scaleBy(glm::vec3{ 10.0f });
	cube->transform.addChild(gizmo);
	gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });

	auto bbDsiplayCube = std::make_shared<DisplayBoundingBoxComponent>(cube, glm::vec3{ 0.0f, 1.0f, 0.0 });
	cube->addComponent(bbDsiplayCube);

	auto bbDisplaySphere = std::make_shared<DisplayBoundingBoxComponent>(sphere, glm::vec3{ 0.0f, 0.0f, 1.0f });
	sphere->addComponent(bbDisplaySphere);

	auto bbDisplayTorus = std::make_shared<DisplayBoundingBoxComponent>(torus, glm::vec3{ 1.0f, 0.0f, 1.0f });
	torus->addComponent(bbDisplayTorus);

	auto bbDsiplay = std::make_shared<DisplayBoundingBoxComponent>(scene);
	scene->addComponent(bbDsiplay);

	auto moveComponent = std::make_shared<MoveComponent>(cube);
	cube->addComponent(bbDsiplay);
	auto camcmp = std::make_shared<CameraComponent>(cube, 1.0471975512f, 1.0f, 10.0f);
	cube->addComponent(camcmp);

	auto showFrustum = std::make_shared<DisplayCameraFrustumComponent>(cube);
	cube->addComponent(showFrustum);

	// Load a cubemap Texture
	auto skyTexture = Texture::loadCubemapFromFile({
					{"front", "test_data/skybox/front.tga"},
					{"back", "test_data/skybox/back.tga"},
					{"top", "test_data/skybox/top.tga"},
					{"bottom", "test_data/skybox/bottom.tga"},
					{"left", "test_data/skybox/left.tga"},
					{"right", "test_data/skybox/right.tga"},
		});

	// Create a Skybox material
	auto skyboxMaterial = std::make_shared<SkyboxMaterial>(skyTexture);

	// Create the actual Skybox
	auto box = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), skyboxMaterial);

	// Create an empy GameObject for the light
	auto sun = Engine::gameObjectManager.createGameObject();
	sun->name = "sun";

	// Lights are implemented as Components
	sun->addComponent(std::make_shared<DirectionalLight>(sun));
	sun->transform.setPosition(glm::vec3{ 50.0f, 205.0f, -65.0f });

	//
	Engine::renderSys.addLight(sun);
	sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::NO_SHADOWS);
	sun->getComponent<Light>()->ambientColor = glm::vec3{ .9f, .9f, .9f } / 15.0f;
	sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f } *5.0f;
	sun->transform.rotateBy(glm::angleAxis(glm::radians(55.0f), sun->transform.right()));

	// Starts the GameLoop
	Engine::start();

	return 0;
}
#endif // LOAD_HIERARCHIES
