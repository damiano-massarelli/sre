#include "rendering/effects/GodRays.h"
#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/Bloom.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/PointLight.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"
#include <glm/gtc/type_ptr.hpp>

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
    godRays->setDensity(1.0F);
    godRays->setDecayRatio(0.96F);
    godRays->lightRadius = 10.0F;
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
        child->transform.setScale(glm::vec3{ 0.5f, 2.5f, 0.1f });

        auto child2 = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), material);
        child2->transform.setParent(parent);
        child2->transform.setLocalPosition(glm::vec3{ 0, i - 4.5f, 0 });
        child2->transform.setScale(glm::vec3{ 2.5f, 0.5f, 0.1f });
    }

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<PointLight>(light));
    light->transform.setPosition(glm::vec3{ 0.0f, 2.0f, 12.0f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };
    light->getComponent<Light>()->specularColor = glm::vec3{ 1.0f, 1.0f, 1.0f };
    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto directionalLight = Engine::gameObjectManager.createGameObject();
    directionalLight->name = "light";
    directionalLight->addComponent(std::make_shared<DirectionalLight>(light));
    directionalLight->transform.setPosition(glm::vec3{ -2.0f, 0.0f, -120.0f });
    Engine::renderSys.addLight(directionalLight);
    directionalLight->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };

    godRays->setLight(directionalLight);

    Engine::uiRenderer.addUIDrawer([godRays, directionalLight]() {
        ImGui::SetNextWindowSizeConstraints(ImVec2{ 320.F, 220.F }, ImVec2{ 320.F, 220.F });
        ImGui::Begin("God Rays Settings");

        ImGui::Text("Radius: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputFloat("##radius",
            &(godRays->lightRadius),
            godRays->isPerspective ? 0.1F : 0.01F,
            1.0F,
            godRays->isPerspective ? 1 : 2);
        ImGui::PopItemWidth();

        ImGui::Text("Perspective: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::Checkbox("##perspective", &(godRays->isPerspective));
        ImGui::PopItemWidth();

        ImGui::Text("Num Samples: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        int samples = godRays->getNumSamples();
        if (ImGui::SliderInt("##samples", &samples, 1, 200)) {
            godRays->setNumSamples(samples);
        }
        ImGui::PopItemWidth();

        ImGui::Text("Decay Ratio: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        float decayRatio = godRays->getDecayRatio();
        if (ImGui::InputFloat("##decay", &decayRatio, 0.01F, 1.0F, 2)) {
            godRays->setDecayRatio(decayRatio);
        }
        ImGui::PopItemWidth();

        ImGui::Text("Density: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        float density = godRays->getDensity();
        if (ImGui::InputFloat("##density", &density, 0.01F, 1.0F, 2)) {
            godRays->setDensity(density);
        }
        ImGui::PopItemWidth();

        ImGui::Text("Weight: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        float weight = godRays->getWeight();
        if (ImGui::InputFloat("##weight", &weight, 0.1F, 1.0F, 1)) {
            godRays->setWeight(weight);
        }
        ImGui::PopItemWidth();

        ImGui::Text("Color: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::ColorEdit3("##lightColor", glm::value_ptr(directionalLight->getComponent<Light>()->diffuseColor));
        ImGui::PopItemWidth();

        ImGui::End();
    });
}

void GodRayTestScene::end() { }
