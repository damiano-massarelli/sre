#include "Engine.h"
#include "Test.h"
#include "cameras/FreeCameraComponent.h"
#include "gameobject/GameObjectLoader.h"
#include "rendering/effects/FXAA.h"
#include "rendering/effects/GammaCorrection.h"
#include "rendering/light/Light.h"
#include "rendering/light/PointLight.h"
#include "rendering/materials/PropMaterial.h"
#include "rendering/materials/SkyboxMaterial.h"
#include "rendering/materials/WaterMaterial.h"
#include "rendering/materials/PBRMaterial.h"
#include "rendering/mesh/MeshCreator.h"
#include "rendering/mesh/MeshLoader.h"
#include "rendering/shadow/ShadowOnVisibleSceneComponent.h"
#include "resourceManagment/RefCount.h"
#include "terrain/HeightMapTerrainHeightProvider.h"
#include "terrain/TerrainGenerator.h"

#include <iostream>

DECLARE_TEST_SCENE("Water Test", WaterTestScene)

void WaterTestScene::start() {
    auto gammaPost = std::make_shared<GammaCorrection>();
    gammaPost->setGamma(2.2F);
    gammaPost->setExposure(1.F);
    Engine::renderSys.effectManager.addEffect(gammaPost);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{ 0.0f, 0.0f, 30.0f });

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    cam->setCameraSensitivity(0.2F);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{ glm::vec3{ 0, glm::radians(180.0f), 0 } });

    Engine::renderSys.setCamera(camera);

   /* auto water = Engine::gameObjectManager.createGameObject(MeshCreator::plane(),
        std::make_shared<WaterMaterial>(-5.0f,
            Texture::loadFromFile("test_data/water/dudv.png"),
            Texture::loadFromFile("test_data/water/normal.png")));

    water->transform.moveBy(glm::vec3{ 35, -5, -5 });
    water->transform.scaleBy(glm::vec3{ 90.0f });
    water->transform.rotateBy(glm::angleAxis(glm::radians(-90.0f), water->transform.right()));*/

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

    // FIXME
    auto groundMaterial = std::make_shared<PBRMaterial>();
    auto appearanceSettings = Texture::AppearanceSettings{};
    appearanceSettings.wrapS = GL_MIRRORED_REPEAT;
    appearanceSettings.wrapT = GL_MIRRORED_REPEAT;
    groundMaterial->setAlbedoMap(Texture::loadFromFile("test_data/terrain/grass.jpg", appearanceSettings));
    groundMaterial->setAlbedo(glm::vec3{1.F});
    groundMaterial->setRoughness(1.F);
    groundMaterial->setMetalness(0.F);
    groundMaterial->useNormalMap(false);

    HeightMapTerrainHeightProvider hProvider{ "test_data/terrain/heightmap.png", -4.5F, 4.5F };
    TerrainGenerator generator{100, 100, 50, 50};
    generator.includeTangentSpace(true);
    generator.setTextureTilesNumber(1.F, 1.F);
    auto terrain = Engine::gameObjectManager.createGameObject(generator.createTerrain(hProvider), groundMaterial);

    Engine::renderSys.effectManager.enableEffects();
    //Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
    Engine::renderSys.shadowMappingSettings.setShadowStrength(0.3f);
    Engine::renderSys.shadowMappingSettings.useFastShader = false;

    auto light = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
    light->name = "light";
    light->addComponent(std::make_shared<DirectionalLight>(light));
    light->transform.setPosition(glm::vec3{ 0.F });
    Engine::renderSys.addLight(light);
    light->getComponent<Light>()->diffuseColor = glm::vec3{ 1.F };
    //light->getComponent<Light>()->ambientColor = glm::vec3{ 0.1F };
    light->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::NO_SHADOWS);
    light->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });

    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(light);
    gizmo->transform.setLocalPosition(glm::vec3{ 0.0f });
    light->transform.rotateBy(glm::angleAxis(glm::radians(90.F), light->transform.right()));
}

void WaterTestScene::end() { }
