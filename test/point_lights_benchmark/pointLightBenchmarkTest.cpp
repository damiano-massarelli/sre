#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/PointLight.h"
#include "rendering/light/Light.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/materials/PBRMaterial.h"
#include <iostream>
#include <vector>
#include <memory>

BEGIN_DECLARE_TEST_SCENE(PointLightsBenchmarkTestScene)
std::int32_t numberOfLights = 0;
std::vector<GameObjectEH> pointLights{};
std::shared_ptr<PropMaterial> propMaterial;
END_DECLARE_TEST_SCENE("Point Lights Benchmark", PointLightsBenchmarkTestScene)

void PointLightsBenchmarkTestScene::start() {
    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());

    propMaterial = std::make_shared<PropMaterial>();

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3(0.0f, 5.f, 30.0f));
    camera->transform.lookAt(glm::vec3{ 0.f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    Engine::renderSys.setCamera(camera);

    auto cube = MeshCreator::cube();

    const auto boxMaterial = std::make_shared<PBRMaterial>();
    boxMaterial->setAlbedo(glm::vec3{ 1.0F, 0.5F, 0.3F });

    for (auto i = 0; i < 16; i++) {
        for (auto j = 0; j < 512 / 16; j++) {
            auto box = Engine::gameObjectManager.createGameObject(cube, boxMaterial);
            box->transform.setPosition(glm::vec3{ 5.F * i, 0.F, 5.F * j });
        }
    }

    Engine::uiRenderer.addUIDrawer([this, cube] {
        ImGui::SetNextWindowSizeConstraints(ImVec2{ 0.f, 0.f }, ImVec2{ 220.f, 190.f });
        ImGui::Begin("Light settings");
        ImGui::Text("Lights: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        int selectedNumberOfLights = numberOfLights;
        ImGui::SliderInt("##numLights", &selectedNumberOfLights, 0, RenderSystem::MAX_LIGHT_NUMBER);

        if (selectedNumberOfLights < numberOfLights) {
            std::for_each(pointLights.begin() + selectedNumberOfLights, pointLights.end(), [](const auto& light) {
                Engine::renderSys.removeLight(light);
            });
            pointLights.erase(pointLights.begin() + selectedNumberOfLights, pointLights.end());
        }

        if (selectedNumberOfLights > numberOfLights) {
            for (auto i = numberOfLights; i < selectedNumberOfLights; ++i) {
                auto row = i % 16;
                auto col = i / 16;
                auto light = Engine::gameObjectManager.createGameObject(cube, propMaterial);
                light->transform.setScale(glm::vec3{ 0.2F });
                light->transform.setPosition(glm::vec3{ 2.5F + 5.F * row, 1.F, 2.5F + 5.F * col });
                auto lightComponent = std::make_shared<PointLight>(light);
                light->addComponent(lightComponent);

                Engine::renderSys.addLight(light);

                pointLights.push_back(light);
            }
        }

        numberOfLights = selectedNumberOfLights;

        ImGui::PopItemWidth();
       
        ImGui::End();
    });

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<DirectionalLight>(light));
    light->transform.setPosition(glm::vec3{ -2.0f, 10.0f, 15.0f });
    light->transform.lookAt(glm::vec3{ 0.f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f } / 100.f;
    light->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::DYNAMIC);

    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light);
    gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });
    gizmo->transform.setLocalRotation(glm::quat{ 1.f, 0.f, 0.f, 0.f });
}

void PointLightsBenchmarkTestScene::end() { }
