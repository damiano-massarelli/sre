#include "rendering/effects/SSR.h"
#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/Bloom.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshCreator.h"

class CameraSpinComponent : public Component, public EventListener {
private:
    float mAngle = 0.F;
    float mRadius = 80.F;

public:
    CameraSpinComponent(GameObjectEH& camera)
        : Component{ camera } {
        Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this);
    }

    void onEvent(SDL_Event e) override {
        const float delta = (*(static_cast<float*>(e.user.data1))) / 1000.0f;
        const float x = mRadius * glm::cos(glm::radians(mAngle));
        const float z = mRadius * glm::sin(glm::radians(mAngle));
        const float y = gameObject->transform.getPosition().y;

        gameObject->transform.setPosition(glm::vec3{ x, y, z });
        gameObject->transform.lookAt(glm::vec3{ 0.F });

        mAngle += 25.F * delta;
    }
};

BEGIN_DECLARE_TEST_SCENE(SSRTestScene)
bool mUseFallbackSkybox = false;
static constexpr char* FLOOR_MATERIALS[] = { "mirror", "tiles", "tiles_green" };
std::int8_t mSelectedFloorMaterial = 0;
bool mUseFallbackSly = false;
bool mSSREnabled = false;
END_DECLARE_TEST_SCENE("SSR", SSRTestScene)

void SSRTestScene::start() {
    auto ssrEffect = std::make_shared<SSR>();
    Engine::renderSys.effectManager.addEffect(std::make_shared<GammaCorrection>());
    Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
    Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>(.55F));
    Engine::renderSys.effectManager.enableEffects();

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 15.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->addComponent(std::make_shared<CameraSpinComponent>(camera));
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

    auto character = GameObjectLoader{}.fromFile("test_data/skeletal_animation/BrainStem.gltf");
    character->getComponent<SkeletalAnimationControllerComponent>()->setCurrentAnimation("default");
    character->getComponent<SkeletalAnimationControllerComponent>()->playCurrentAnimation();
    character->getComponent<SkeletalAnimationControllerComponent>()->getCurrentAnimation()->loopDirection
        = SkeletalAnimation::LoopDirection::BOUNCE;
    character->transform.scaleBy(glm::vec3{ 10.F });

    auto trashbin = GameObjectLoader{}.fromFile("test_data/trashbin/trashbin.glb");
    trashbin->transform.moveBy(glm::vec3{ 15.F, 0.F, 15.F });

    auto vendingMachine = GameObjectLoader{}.fromFile("test_data/vendingmachine/vendingmachine.glb");
    vendingMachine->transform.moveBy(glm::vec3{ -20.F, 0.F, -15.F });
    vendingMachine->transform.rotateBy(glm::angleAxis(glm::radians(-35.F), glm::vec3{ 0.F, 1.F, 0.F }));

    auto oldVendingMachine = GameObjectLoader{}.fromFile("test_data/vendingmachine/oldvendingmachine.glb");
    oldVendingMachine->transform.moveBy(glm::vec3{ 10.F, 0.F, -17.F });
    oldVendingMachine->transform.rotateBy(glm::angleAxis(glm::radians(-195.F), glm::vec3{ 0.F, 1.F, 0.F }));

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

    auto sun = Engine::gameObjectManager.createGameObject();
    sun->name = "sun";

    // Lights
    sun->addComponent(std::make_shared<DirectionalLight>(sun));
    sun->transform.setPosition(glm::vec3{ 25.F, 15.F, -5.F });
    Engine::renderSys.addLight(sun);
    sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::DYNAMIC);
    sun->getComponent<Light>()->diffuseColor = glm::vec3{ 5.0f, 5.0f, 5.0f };
    sun->getComponent<Light>()->ambientColor = glm::vec3{ 1.F };
    sun->transform.lookAt(glm::vec3{ 0.F });

    // Ground
    std::shared_ptr<PBRMaterial> planeMaterial = std::make_shared<PBRMaterial>();
    planeMaterial->setUVScale(glm::vec2{ 8.F });

    GameObjectEH plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), planeMaterial);
    plane->transform.setRotation(glm::angleAxis(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)));
    plane->transform.scaleBy(glm::vec3(180.f));

    Engine::uiRenderer.addUIDrawer([ssrEffect, planeMaterial, skyTexture, this]() {
        float maxDistance = ssrEffect->getMaxReflectionDistance();
        std::int32_t numSamples = ssrEffect->getNumSamples();
        int steps = ssrEffect->getSteps();
        float hitThreshold = ssrEffect->geHitThreshold();
        float steepAngleHitThresholdMult = ssrEffect->getSteepAngleHitThresholdMultiplier();

        float roughness = planeMaterial->getRoughness();

        ImGui::Begin("Settings");
        ImGui::Text("SSR Settings");
        if (ImGui::Checkbox("Enabled", &mSSREnabled)) {
            if (mSSREnabled) {
                Engine::renderSys.effectManager.addEffectBefore<GammaCorrection>(ssrEffect);
            } else {
                Engine::renderSys.effectManager.removeEffect(ssrEffect);
            }
        }
        if (ImGui::SliderFloat("Max Distance", &maxDistance, 0.f, 500.f)) {
            ssrEffect->setMaxReflectionDistance(maxDistance);
        }
        if (ImGui::SliderInt("Samples", &numSamples, 10, 200)) {
            ssrEffect->setNumSamples(numSamples);
        }
        if (ImGui::SliderInt("Steps", &steps, 0, 130)) {
            ssrEffect->setSteps(steps);
        }
        if (ImGui::SliderFloat("Hit Threshold", &hitThreshold, 0.01f, 10.f)) {
            ssrEffect->setHitThreshold(hitThreshold);
        }
        if (ImGui::SliderFloat("Steep Angle Hit Threshold Mult", &steepAngleHitThresholdMult, 1.f, 10.f)) {
            ssrEffect->setSteepAngleHitThresholdMultiplier(steepAngleHitThresholdMult);
        }

        if (ImGui::Checkbox("Use fallback skybox", &mUseFallbackSkybox)) {
            if (mUseFallbackSkybox) {
                ssrEffect->setFallbackSkyboxTexture(skyTexture);
            } else {
                ssrEffect->setFallbackSkyboxTexture(Texture{});
            }
        }

        ImGui::Separator();
        ImGui::Text("Floor Settings");

        if (ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f)) {
            planeMaterial->setRoughness(roughness);
        }

        constexpr Texture::AppearanceSettings settings{ true, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
        if (ImGui::BeginCombo("Material", FLOOR_MATERIALS[mSelectedFloorMaterial])) {
            for (int i = 0; i < IM_ARRAYSIZE(FLOOR_MATERIALS); ++i) {
                if (ImGui::Selectable(FLOOR_MATERIALS[i], i == mSelectedFloorMaterial, 0)) {
                    mSelectedFloorMaterial = i;

                    std::string basePath = std::string{ "test_data/ssr/" } + FLOOR_MATERIALS[i];
                    std::string albedoMap = basePath + "/albedo.jpg";
                    std::string roughnessMap = basePath + "/roughness.jpg";
                    std::string metalnessMap = basePath + "/metalness.jpg";
                    std::string aoMap = basePath + "/ao.jpg";
                    std::string normalMap = basePath + "/normal.jpg";

                    planeMaterial->setAlbedoMap(Texture::loadFromFile(albedoMap, settings));
                    planeMaterial->setRoughnessMap(Texture::loadFromFile(roughnessMap, settings));
                    planeMaterial->setMetalnessMap(Texture::loadFromFile(metalnessMap, settings));
                    planeMaterial->setAmbientOcclusionMap(Texture::loadFromFile(aoMap, settings));
                    planeMaterial->setNormalMap(Texture::loadFromFile(normalMap, settings));
                }
            }

            ImGui::EndCombo();
        }

        ImGui::End();
    });
}

void SSRTestScene::end() { }
