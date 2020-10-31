#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/light/PointLight.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include <iostream>
#include <memory>
#include <random>
#include <vector>

BEGIN_DECLARE_TEST_SCENE(PointLightsBenchmarkTestScene)
std::int32_t numberOfLights = 0;
std::vector<GameObjectEH> pointLights{};
float attenuationConstant = 1.0f;
float attenuationLinear = 0.045f;
float attenuationQuadratic = 0.0075f;
END_DECLARE_TEST_SCENE("Point Lights Benchmark", PointLightsBenchmarkTestScene)

void PointLightsBenchmarkTestScene::start() {
    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3(0.0f, 5.f, 30.0f));
    camera->transform.lookAt(glm::vec3{ 0.f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    Engine::renderSys.setCamera(camera);

    auto cube = MeshCreator::cube();

    const auto boxMaterial = std::make_shared<PBRMaterial>();
    boxMaterial->setAlbedo(glm::vec3{ 1.0F, 1.0F, 1.0F });

    const auto planeMesh = MeshCreator::plane();
    const auto plane = Engine::gameObjectManager.createGameObject(planeMesh, boxMaterial);
    plane->transform.scaleBy(glm::vec3{ 5.0F * 15 + 1.F, 5.0F * 15 + 1.F, 1.0F });
    plane->transform.rotateBy(glm::angleAxis(glm::radians(-90.F), glm::vec3{ 1.0F, 0.0F, 0.0F }));
    plane->transform.moveBy(glm::vec3{ 2.5F * 15, -0.5F, 2.5F * 15 });

    for (auto i = 0; i < 16; i++) {
        for (auto j = 0; j < RenderSystem::MAX_LIGHT_NUMBER / 16; j++) {
            auto box = Engine::gameObjectManager.createGameObject(cube, boxMaterial);
            box->transform.setPosition(glm::vec3{ 5.F * i, 0.F, 5.F * j });
        }
    }

    Engine::uiRenderer.addUIDrawer([this, cube] {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0F, 1.0F);

        ImGui::SetNextWindowSizeConstraints(ImVec2{ 0.f, 0.f }, ImVec2{ 280.f, 250.f });
        ImGui::Begin("Light settings");
        ImGui::Text("Lights: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        int selectedNumberOfLights = numberOfLights;
        ImGui::SliderInt("##numLights",
            &selectedNumberOfLights,
            0,
            RenderSystem::MAX_LIGHT_NUMBER - 1);  // -1: there is the directional light

        if (selectedNumberOfLights < numberOfLights) {
            std::for_each(pointLights.begin() + selectedNumberOfLights, pointLights.end(), [](const auto& light) {
                Engine::renderSys.removeLight(light);
                Engine::gameObjectManager.remove(light);
            });
            pointLights.erase(pointLights.begin() + selectedNumberOfLights, pointLights.end());
        }

        if (selectedNumberOfLights > numberOfLights) {
            for (auto i = numberOfLights; i < selectedNumberOfLights; ++i) {
                auto row = i % 16;
                auto col = i / 16;
                auto propMaterial = std::make_shared<PropMaterial>();
                propMaterial->color = glm::vec3{ dis(gen), dis(gen), dis(gen) };
                auto light = Engine::gameObjectManager.createGameObject(cube, propMaterial);
                light->transform.setScale(glm::vec3{ 0.2F });
                light->transform.setPosition(glm::vec3{ 2.5F + 5.F * row, 1.F, 2.5F + 5.F * col });

                auto lightComponent = std::make_shared<PointLight>(light);
                lightComponent->diffuseColor = propMaterial->color;
                lightComponent->attenuationConstant = attenuationConstant;
                lightComponent->attenuationLinear = attenuationLinear;
                lightComponent->attenuationQuadratic = attenuationQuadratic;

                light->addComponent(lightComponent);

                Engine::renderSys.addLight(light);

                pointLights.push_back(light);
            }
        }
        numberOfLights = selectedNumberOfLights;

        auto sliderChanged = false;
        ImGui::Text("const. atten.: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        sliderChanged |= ImGui::SliderFloat("##consAtt", &attenuationConstant, 0.F, 10.F);
        ImGui::PopItemWidth();

        ImGui::Text("linear atten.: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        sliderChanged |= ImGui::SliderFloat("##linearAtten", &attenuationLinear, 0.F, 10.F);
        ImGui::PopItemWidth();

        ImGui::Text("quadr. atten.: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        sliderChanged |= ImGui::SliderFloat("##quadAtten", &attenuationQuadratic, 0.F, 10.F);
        ImGui::PopItemWidth();

        if (sliderChanged) {
            for (const auto light : pointLights) {
                auto lightComponent = light->getComponent<Light>();
                lightComponent->attenuationConstant = attenuationConstant;
                lightComponent->attenuationLinear = attenuationLinear;
                lightComponent->attenuationQuadratic = attenuationQuadratic;
            }
        }

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

    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light);
    gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });
    gizmo->transform.setLocalRotation(glm::quat{ 1.f, 0.f, 0.f, 0.f });
}

void PointLightsBenchmarkTestScene::end() { }
