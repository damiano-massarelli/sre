#include "Engine.h"
#include "Test.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/mesh/MeshLoader.h"

#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/Bloom.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/MotionBlur.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/particle/ParticleEmitter.h"

#include <iostream>

static void addParticles(const GameObjectEH& eh) {
    EmitterSettings settings;
    settings.velocityMin = glm::vec3{ 0.0f, -1.0f, 0.0f };
    settings.velocityMax = glm::vec3{ 0.0f, 0.0f, 0.0f };
    settings.positionOffsetMin = -glm::vec3{ 1.0f, 0.0f, 1.0f };
    settings.positionOffsetMax = glm::vec3{ 1.0f, 0.5f, 1.0f };
    settings.minDuration = 500.0f;
    settings.maxDuration = 1500.0f;
    settings.finalScaleMin = 0.0f;
    settings.finalScaleMax = 0.1f;
    settings.initialRotationMin = glm::radians(-30.0f);
    settings.initialRotationMax = glm::radians(30.0f);
    settings.finalRotationMin = glm::radians(-30.0f);
    settings.finalRotationMax = glm::radians(30.0f);
    settings.initialGravityScaleMin = -0.3f;
    settings.initialGravityScaleMax = -1.0f;
    settings.finalGravityScaleMin = -1.3f;
    settings.finalGravityScaleMax = -1.9f;
    settings.dfactor = GL_ONE;

    eh->addComponent(std::make_shared<ParticleEmitter>(eh, 500));

    eh->getComponent<ParticleEmitter>()->setTextureAtlas(
        Texture::loadFromFile("test_data/particle/fire.png"), 25, 5, 5);
    eh->getComponent<ParticleEmitter>()->start(300.0f);
    eh->getComponent<ParticleEmitter>()->settings = settings;

    eh->transform.moveBy(glm::vec3{ 0, 1.0f, 0 });
    eh->addComponent(std::make_shared<PointLight>(eh));
    eh->getComponent<PointLight>()->setCastShadowMode(Light::ShadowCasterMode::NO_SHADOWS);
    eh->getComponent<PointLight>()->diffuseColor = glm::vec3{ 0.89f, 0.75f, 0.276f };
    eh->getComponent<PointLight>()->specularColor = glm::vec3{ 0.89f, 0.75f, 0.276f };
    Engine::renderSys.addLight(eh);
}

DECLARE_TEST_SCENE("Particles", ParticleTestScene)

void ParticleTestScene::start() {
    Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
    Engine::renderSys.effectManager.addEffect(std::make_shared<MotionBlur>());

    Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>());

    Engine::renderSys.effectManager.enableEffects();
    Engine::renderSys.shadowMappingSettings.useFastShader = true;
    Engine::renderSys.shadowMappingSettings.width = 500;
    Engine::renderSys.shadowMappingSettings.height = 500;
    Engine::renderSys.shadowMappingSettings.setShadowStrength(1.0f);
    Engine::renderSys.shadowMappingSettings.setShadowDistance(500);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

    auto sponza = GameObjectLoader().fromFile("test_data/bloom/sponza.fbx");
    sponza->transform.setScale(glm::vec3{ 0.1f });

    for (const auto& eh : sponza->transform.findAll("firePos"))
        addParticles(eh);

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

    // auto human =
    // GameObjectLoader().fromFile("C:/Users/damia/Desktop/warrior_idle.dae");
    // auto animationController =
    // human->getComponent<SkeletalAnimationControllerComponent>();
    // animationController->playAnimation("default");
    // auto animation = animationController->getAnimation("default");
    // animation->loopDirection = SkeletalAnimation::LoopDirection::REPEAT;

    // human->transform.scaleBy(glm::vec3{ 10.0f });

    auto sun = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    sun->name = "sun";
    sun->addComponent(std::make_shared<DirectionalLight>(sun));
    sun->transform.setPosition(glm::vec3{ 50.0f, 205.0f, -65.0f });
    Engine::renderSys.addLight(sun);
    sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
    sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f };
    sun->getComponent<Light>()->specularColor = glm::vec3{ .9f, .9f, .9f };
    sun->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
    sun->transform.rotateBy(glm::angleAxis(glm::radians(55.0f), sun->transform.right()));
    sun->transform.rotateBy(glm::angleAxis(glm::radians(-15.0f), sun->transform.up()));

    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(sun);
    gizmo->transform.setLocalRotation(glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f });
    gizmo->transform.setPosition(sun->transform.getPosition());

    auto fakeSun = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    fakeSun->name = "fakeSun";
    fakeSun->addComponent(std::make_shared<DirectionalLight>(fakeSun));
    fakeSun->transform.setPosition(glm::vec3{ -10.0f, 105, 10.0f });
    Engine::renderSys.addLight(fakeSun);
    fakeSun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
    fakeSun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f } / 10.0f;
    fakeSun->getComponent<Light>()->specularColor = glm::vec3{ .9f, .9f, .9f } / 10.0f;
    fakeSun->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
    fakeSun->transform.rotateBy(glm::angleAxis(glm::radians(155.0f), fakeSun->transform.right()));

    auto gizmo2 = MeshCreator::axisGizmo();
    gizmo2->transform.setParent(fakeSun);
    gizmo2->transform.setLocalRotation(glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f });
    gizmo2->transform.setPosition(fakeSun->transform.getPosition());
}

void ParticleTestScene::end() { }
