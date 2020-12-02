#include "rendering/effects/SSR.h"
#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"

DECLARE_TEST_SCENE("SSR", SSRTestScene)

void SSRTestScene::start() {
    auto ssrEffect = std::make_shared<SSR>();
    Engine::renderSys.effectManager.addEffect(ssrEffect);
    Engine::renderSys.effectManager.enableEffects();

    auto gammaPost = std::make_shared<GammaCorrection>();
    gammaPost->setGamma(2.2f);
    gammaPost->setExposure(1.0f);
    Engine::renderSys.effectManager.addEffect(gammaPost);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 30.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

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

    auto sun = Engine::gameObjectManager.createGameObject();
    sun->name = "sun";

    // Lights
    sun->addComponent(std::make_shared<DirectionalLight>(sun));
    sun->transform.setPosition(glm::vec3{ 0.0f, 5.0f, 0.0f });
    Engine::renderSys.addLight(sun);
    sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
    sun->getComponent<Light>()->diffuseColor = glm::vec3{ 10.0f, 10.0f, 10.0f };
    sun->getComponent<Light>()->ambientColor = glm::vec3{ 0.1F };
    sun->transform.rotateBy(glm::angleAxis(glm::radians(55.0f), sun->transform.right()));

    // Ground
    Texture::AppearanceSettings settings;
    settings.wrapS = GL_REPEAT;
    settings.wrapT = GL_REPEAT;
    std::shared_ptr<PBRMaterial> planeMaterial = std::make_shared<PBRMaterial>();
    planeMaterial->setAlbedoMap(Texture::loadFromFile("test_data/ssr/textures/checker.png", settings));
    planeMaterial->setAlbedo(glm::vec3(1.f));
    planeMaterial->setUVScale(glm::vec2{ 8.F });
    //planeMaterial->setRoughnessMap(Texture::loadFromFile("test_data/ssr/textures/metal/roughness.jpg"));
    //planeMaterial->setRoughnessMap(Texture::loadFromFile("test_data/ssr/textures/checker.png"));
    //planeMaterial->setMetalnessMap(Texture::loadFromFile("test_data/ssr/textures/metal/metalness.jpg"));
    //planeMaterial->setMetalness(0.f);
    //planeMaterial->setNormalMap(Texture::loadFromFile("test_data/ssr/textures/metal/normal.jpg"));
    //planeMaterial->setAmbientOcclusionMap(Texture::loadFromFile("test_data/ssr/textures/metal/ao.jpg"));

    GameObjectEH plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), planeMaterial);
    plane->transform.setRotation(glm::angleAxis(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)));
    plane->transform.scaleBy(glm::vec3(180.f));

    // Sphere
    std::shared_ptr<PBRMaterial> redMaterial = std::make_shared<PBRMaterial>();
    redMaterial->setAlbedo(glm::vec3{ 1.0, 0.0, 0.0 });

    GameObjectEH sphere = Engine::gameObjectManager.createGameObject(MeshCreator::sphere(), redMaterial);
    sphere->transform.setPosition(glm::vec3(0.f, 5.f, 0.f));
    sphere->transform.scaleBy(glm::vec3(5.f));

    // Cube
    const auto cube = MeshCreator::cube();
    const auto cubeEH = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), redMaterial);
    cubeEH->transform.scaleBy(glm::vec3{ 5.F });
    cubeEH->transform.moveBy(glm::vec3{ 10.F, 2.5F, 0.F });

    const auto leftWall = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), redMaterial);
    leftWall->transform.scaleBy(glm::vec3{ 3.F, 20.F, 180.F });
    leftWall->transform.moveBy(glm::vec3{ -90.F, 10.F, 0.F });

    Engine::uiRenderer.addUIDrawer([ssrEffect, cam, redMaterial]() {
        float maxDistance = ssrEffect->getMaxDistance();
        std::int32_t numSamples = ssrEffect->getNumSamples();
        int steps = ssrEffect->getSteps();
        float hitThreshold = ssrEffect->geHitThreshold();

        float roughness = redMaterial->getRoughness();

        ImGui::Begin("Settings");
        if (ImGui::SliderFloat("Max Distance", &maxDistance, 0.f, 500.f)) {
            ssrEffect->setMaxDistance(maxDistance);
        }
        if (ImGui::SliderInt("Samples", &numSamples, 10, 200)) {
            ssrEffect->setNumSamples(numSamples);
        }
        if (ImGui::SliderInt("Steps", &steps, 0, 30)) {
            ssrEffect->setSteps(steps);
        }
        if (ImGui::SliderFloat("Hit Threshold", &hitThreshold, 0.01f, 10.f)) {
            ssrEffect->setHitThreshold(hitThreshold);
        }
        if (ImGui::SliderFloat("Roughness", &roughness, 0.f, 1.f)) {
            redMaterial->setRoughness(roughness);
        }

        ImGui::End();
    });
}

void SSRTestScene::end() { }
