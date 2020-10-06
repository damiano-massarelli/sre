#include "Engine.h"
#include "Test.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/effects/GammaCorrection.h"
#include "cameras/FreeCameraComponent.h"
#include "rendering/light/Light.h"
#include "rendering/mesh/MeshCreator.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/SSR.h"
#include "rendering/materials/SkyboxMaterial.h"

#include <iostream>

DECLARE_TEST_SCENE("Screen Space Reflections", SSRTestScene)

void SSRTestScene::start() {
    //Engine::renderSys.effectManager.addEffect(std::make_shared<SSR>());
    auto gammaPost = std::make_shared<GammaCorrection>();
    gammaPost->setGamma(2.2f);
    gammaPost->setExposure(1.0f);
    Engine::renderSys.effectManager.addEffect(gammaPost);
    Engine::renderSys.effectManager.enableEffects();

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 30.0f, 30.0f});

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});

    Engine::renderSys.setCamera(camera);

    //GameObjectEH scene = GameObjectLoader{}.fromFile("test_data/ssr/scene.fbx");

    auto skyTexture = Texture::loadCubemapFromFile({
                   {"front", "test_data/skybox/front.tga"},
                   {"back", "test_data/skybox/back.tga"},
                   {"top", "test_data/skybox/top.tga"},
                   {"bottom", "test_data/skybox/bottom.tga"},
                   {"left", "test_data/skybox/left.tga"},
                   {"right", "test_data/skybox/right.tga"},
        });
    auto skyboxMaterial = std::make_shared<SkyboxMaterial>(skyTexture);
    auto box = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), skyboxMaterial);

    auto sun = Engine::gameObjectManager.createGameObject();
    sun->name = "sun";

    // Lights are implemented as Components
    sun->addComponent(std::make_shared<DirectionalLight>(sun));
    sun->transform.setPosition(glm::vec3{ 0.0f, 5.0f, 0.0f });
    Engine::renderSys.addLight(sun);
    sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::NO_SHADOWS);
    sun->getComponent<Light>()->ambientColor = glm::vec3{ .9f, .9f, .9f } / 15.0f;
    sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f } * 1.0f;
    sun->transform.rotateBy(glm::angleAxis(glm::radians(55.0f), sun->transform.right()));

    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(sun);
    gizmo->transform.setLocalPosition(glm::vec3{0.0f});
    gizmo->transform.setLocalRotation(glm::quat{0.f, 0.f, 0.f, 1.f});

    // Ground
    std::shared_ptr<PBRMaterial> planeMat = std::make_shared<PBRMaterial>();
    planeMat->setAlbedoMap(Texture::loadFromFile("test_data/ssr/textures/metal/albedo.jpg"));
    planeMat->setRoughnessMap(Texture::loadFromFile("test_data/ssr/textures/metal/roughness.jpg"));
    planeMat->setMetalnessMap(Texture::loadFromFile("test_data/ssr/textures/metal/metalness.jpg"));
    planeMat->setNormalMap(Texture::loadFromFile("test_data/ssr/textures/metal/normal.jpg"));
    planeMat->setAmbientOcclusionMap(Texture::loadFromFile("test_data/ssr/textures/metal/ao.jpg"));
    
    GameObjectEH plane = Engine::gameObjectManager.createGameObject(MeshCreator::plane(), planeMat);
    plane->transform.setRotation(glm::angleAxis(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)));
    plane->transform.scaleBy(glm::vec3(30.f));
}

void SSRTestScene::end() {}
