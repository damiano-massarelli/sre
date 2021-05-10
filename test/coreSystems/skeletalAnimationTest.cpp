#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/Light.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"
#include "skeletalAnimation/SkeletalAnimationControllerComponent.h"
#include <iostream>

BEGIN_DECLARE_TEST_SCENE(SkeletalAnimationTestScene)
GameObjectEH object;
std::int32_t chosenModelIndex = 0;
END_DECLARE_TEST_SCENE("Skeletal Animation", SkeletalAnimationTestScene)

void SkeletalAnimationTestScene::start() {
    Engine::renderSys.shadowMappingSettings.useFastShader = false;
    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());
    Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());

    Engine::renderSys.shadowMappingSettings.width = 5.f;
    Engine::renderSys.shadowMappingSettings.height = 5.f;

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3(0.0f, 5.f, 30.0f));
    camera->transform.lookAt(glm::vec3{ 0.f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    Engine::renderSys.setCamera(camera);

    auto skyTexture = Texture::loadCubemapFromFile({
        { "front", "test_data/skybox/front.tga" },
        { "back", "test_data/skybox/back.tga" },
        { "top", "test_data/skybox/top.tga" },
        { "bottom", "test_data/skybox/bottom.tga" },
        { "left", "test_data/skybox/left.tga" },
        { "right", "test_data/skybox/right.tga" },
    });
    auto skyboxMaterial = std::make_shared<SkyboxMaterial>(skyTexture);
    auto box = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), skyboxMaterial);

    const auto plane = MeshCreator::plane();
    const auto gridMaterial = std::make_shared<PBRMaterial>();
    gridMaterial->setAlbedoMap(Texture::loadFromFile("test_data/textures/grid-texture.jpg"));
    auto grid = Engine::gameObjectManager.createGameObject(plane, gridMaterial);
    grid->transform.scaleBy(glm::vec3{ 25.f });
    grid->transform.rotateBy(glm::angleAxis(glm::radians(-90.f), glm::vec3{ 1.f, 0.f, 0.f }));

    object = GameObjectLoader{}.fromFile("test_data/skeletal_animation/RiggedSimple.gltf");
    object->getComponent<SkeletalAnimationControllerComponent>()->setCurrentAnimation("default");
    assert(object.isValid());

    Engine::uiRenderer.addUIDrawer([this] {
        ImGui::SetNextWindowSizeConstraints(ImVec2{ 0.f, 0.f }, ImVec2{ 220.f, 190.f });
        ImGui::Begin("Animation Settings");

        // Model Selection
        constexpr char* modelNames[]{ "RiggedSimple", "RiggedFigure", "BrainStem" };
        ImGui::Text("Model: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##model", modelNames[chosenModelIndex])) {
            const ImVec2 size{ 135.f, 0.f };
            for (auto i = 0; i < 3; ++i) {
                auto selectedModel = chosenModelIndex;
                if (ImGui::Selectable(modelNames[i], chosenModelIndex == i, 0, size)) {
                    selectedModel = i;
                }

                if (selectedModel != chosenModelIndex) {
                    Engine::gameObjectManager.remove(object);
                    chosenModelIndex = selectedModel;
                    object = GameObjectLoader{}.fromFile(
                        "test_data/skeletal_animation/" + std::string{ modelNames[chosenModelIndex] } + ".gltf");
                    object->getComponent<SkeletalAnimationControllerComponent>()->setCurrentAnimation("default");
                }
            }

            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        ImGui::Separator();

        // Get skeletal animation controller
        auto skeletalAnimController = object->getComponent<SkeletalAnimationControllerComponent>();
        auto currentAnimation = skeletalAnimController->getCurrentAnimation();
        if (currentAnimation == nullptr) {
            ImGui::End();
            return;
        }

        const float duration = currentAnimation->getDuration();
        SkeletalAnimation::LoopDirection loopDirection = currentAnimation->loopDirection;

        ImGui::Text("Duration %f seconds", duration);

        ImGui::Text("Loop direction: ");
        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        constexpr char* loopDirectionNames[]{ "Stop", "Repeat", "Bounce" };
        if (ImGui::BeginCombo("##loopDirection", loopDirectionNames[static_cast<std::int8_t>(loopDirection)])) {
            const ImVec2 size{ 75.f, 0.f };
            if (ImGui::Selectable("Stop", loopDirection == SkeletalAnimation::LoopDirection::STOP, 0, size)) {
                loopDirection = SkeletalAnimation::LoopDirection::STOP;
            }
            if (ImGui::Selectable("Repeat", loopDirection == SkeletalAnimation::LoopDirection::REPEAT, 0, size)) {
                loopDirection = SkeletalAnimation::LoopDirection::REPEAT;
            }
            if (ImGui::Selectable("Bounce", loopDirection == SkeletalAnimation::LoopDirection::BOUNCE, 0, size)) {
                loopDirection = SkeletalAnimation::LoopDirection::BOUNCE;
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        currentAnimation->loopDirection = loopDirection;

        ImGui::Text("Speed: ");
        ImGui::SameLine();
        float playbackSpeed = currentAnimation->playbackSpeed;
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::SliderFloat("##playbackSpeed", &playbackSpeed, 0.f, 5.f);
        ImGui::PopItemWidth();
        currentAnimation->playbackSpeed = playbackSpeed;

        ImGui::Separator();

        // playback Buttons
        const bool displayPlay = !skeletalAnimController->isCurrentAnimationPlaying();
        const bool playPausePressed = ImGui::Button(
            displayPlay ? "Play" : "Pause", ImVec2{ ImGui::GetContentRegionAvail().x / 2.f - 2.5f, 0.f });
        ImGui::SameLine(0.f, -5.f);
        const bool stopPressed = ImGui::Button("Stop", ImVec2{ ImGui::GetContentRegionAvail().x, 0.f });

        float time = skeletalAnimController->getCurrentAnimationTime();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::SliderFloat("##time", &time, 0.f, currentAnimation->getDuration(), "%.3f seconds")) {
            skeletalAnimController->setCurrentAnimationTime(time);
        }
        ImGui::PopItemWidth();

        // playback buttons handling
        if (playPausePressed) {
            if (displayPlay) {
                skeletalAnimController->playCurrentAnimation();
            } else {
                skeletalAnimController->pauseCurrentAnimation();
            }
        }

        if (stopPressed) {
            skeletalAnimController->stopCurrentAnimation();
        }

        ImGui::End();
    });

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<DirectionalLight>(light));
    light->transform.setPosition(glm::vec3{ -2.0f, 10.0f, 15.0f });
    light->transform.lookAt(glm::vec3{ 0.f });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };
    light->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::DYNAMIC);

    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light);
    gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });
    gizmo->transform.setLocalRotation(glm::quat{ 1.f, 0.f, 0.f, 0.f });
}

void SkeletalAnimationTestScene::end() { }
