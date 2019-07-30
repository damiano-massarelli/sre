#include "Engine.h"
#include "MeshLoader.h"
#include "BlinnPhongMaterial.h"
#include "PropMaterial.h"

#include "FreeCameraComponent.h"
#include "DirectionalLight.h"
#include "MeshCreator.h"
#include "GameObjectLoader.h"
#include "FXAA.h"
#include "Bloom.h"
#include "runTest.h"
#include "ParticleEmitter.h"
#include "SkyboxMaterial.h"

#include <iostream>


void addParticles(const GameObjectEH& eh) {
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

	eh->getComponent<ParticleEmitter>()->setTextureAtlas(Texture::loadFromFile("test_data/particle/fire.png"), 25, 5, 5);
	eh->getComponent<ParticleEmitter>()->start(300.0f);
	eh->getComponent<ParticleEmitter>()->settings = settings;

	eh->transform.moveBy(glm::vec3{ 0, 1.0f, 0 });
	eh->addComponent(std::make_shared<PointLight>(eh));
	eh->getComponent<PointLight>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
	eh->getComponent<PointLight>()->ambientColor = glm::vec3{ 0.89f, 0.75f, 0.276f } / 5.0f;
	eh->getComponent<PointLight>()->diffuseColor = glm::vec3{ 0.89f, 0.75f, 0.276f };
	eh->getComponent<PointLight>()->specularColor = glm::vec3{ 0.89f, 0.75f, 0.276f };
	Engine::renderSys.addLight(eh);
}

#ifdef particleTest
int main(int argc, char* argv[]) {
    Engine::init();

    Engine::renderSys.createWindow(1280, 720);
	Engine::renderSys.effectManager.addEffect(std::make_shared<FXAA>());
	Engine::renderSys.effectManager.addEffect(std::make_shared<Bloom>());
// 
// 
 	Engine::renderSys.effectManager.enableEffects();
	Engine::renderSys.shadowMappingSettings.useFastShader = true;
	Engine::renderSys.shadowMappingSettings.width = 500;
	Engine::renderSys.shadowMappingSettings.height = 500;
	Engine::renderSys.shadowMappingSettings.setShadowStrength(1.0f);
	Engine::renderSys.shadowMappingSettings.setShadowDistance(500);

    auto camera = Engine::gameObjectManager.createGameObject();
    camera->name = "camera";
    camera->transform.moveBy(glm::vec3{0.0f, 0.0f, 30.0f});
    Engine::renderSys.camera = camera;

    auto cam = std::make_shared<FreeCameraComponent>(camera);
    camera->addComponent(cam);
    camera->transform.setRotation(glm::quat{glm::vec3{0, glm::radians(180.0f), 0}});

	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		std::cout << "before sponza " << error << "\n";
	auto sponza = GameObjectLoader().fromFile("test_data/bloom/sponza.fbx");
	sponza->transform.setScale(glm::vec3{ 0.1f });

	while ((error = glGetError()) != GL_NO_ERROR)
		std::cout << "before particles " << error << "\n";
	for (const auto& eh : sponza->transform.findAll("firePos"))
		addParticles(eh);
	while ((error = glGetError()) != GL_NO_ERROR)
		std::cout << "after -- " << error << "\n";

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

	auto human = GameObjectLoader().fromFile("C:/Users/damia/Desktop/warrior_idle.dae");
	auto animationController = human->getComponent<SkeletralAnimationControllerComponent>();
	animationController->playAnimation("default");
	auto animation = animationController->getAnimation("default");
	animation->loopDirection = SkeletalAnimation::LoopDirection::REPEAT;

	human->transform.scaleBy(glm::vec3{ 10.0f });

	auto sun = Engine::gameObjectManager.createGameObject(MeshCreator::cube(), std::make_shared<PropMaterial>());
	sun->name = "sun";
	sun->addComponent(std::make_shared<DirectionalLight>(sun));
	sun->transform.setPosition(glm::vec3{ 50.0f, 205.0f, -65.0f });
	Engine::renderSys.addLight(sun);
	sun->getComponent<Light>()->setCastShadowMode(Light::ShadowCasterMode::STATIC);
	sun->getComponent<Light>()->ambientColor = glm::vec3{ .9f, .9f, .9f } / 15.0f;
	sun->getComponent<Light>()->diffuseColor = glm::vec3{ .9f, .9f, .9f };
	sun->getComponent<Light>()->specularColor = glm::vec3{ .9f, .9f, .9f };
	sun->transform.scaleBy(glm::vec3{ 0.2f, 0.2f, 0.2f });
	sun->transform.rotateBy(glm::angleAxis(glm::radians(55.0f), sun->transform.right()));
	sun->transform.rotateBy(glm::angleAxis(glm::radians(-15.0f), sun->transform.up()));


    auto gizmo = MeshCreator::axisGizmo();
    gizmo->transform.setParent(sun);
	gizmo->transform.setLocalRotation(glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f });
    gizmo->transform.setPosition(sun->transform.getPosition());

    Engine::start();

    return 0;
}
#endif // LOAD_HIERARCHIES
