#include "WaterMaterial.h"
#include "RenderSystem.h"
#include "Engine.h"
#include "RenderPhase.h"
#include <glm/glm.hpp>

WaterMaterial::WaterMaterial(float waterY, const Texture& dudv)
	: Material{"shaders/waterVS.glsl", "shaders/waterFS.glsl"}, mWaterY{ waterY }, mDuDvMap{ dudv }
{
	mReflectionTarget.create(640, 360);
	mReflectionFbo.init(640, 360);

	mRefractionTarget.create(1280, 720);
	mRefractionFbo.init(1280, 720);

	mReflectionCamera = Engine::gameObjectManager.createGameObject();

	// don't render when rendering for water or shadows
	unSupportedRenderPhases |= RenderPhase::ALL & ~RenderPhase::FORWARD_RENDERING;

	mEventCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
	shader.use();
	shader.setInt("reflection", 0);
	shader.setInt("refraction", 1);
	shader.setInt("dudv", 2);
	shader.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);

	mMoveDuDvLocation = shader.getLocationOf("moveDuDv");
}

void WaterMaterial::onEvent(SDL_Event e)
{


	auto oldDeferredFbo = Engine::renderSys.deferredRenderingFBO;
	Engine::renderSys.enableClipPlane();

	renderRefraction();

	renderReflection();

	Engine::renderSys.disableClipPlane();
	Engine::renderSys.deferredRenderingFBO = oldDeferredFbo;
}

void WaterMaterial::renderReflection()
{
	GameObjectEH oldCamera = Engine::renderSys.camera;
	mReflectionCamera->transform = oldCamera->transform;

	// temp camera for this stage
	Engine::renderSys.camera = mReflectionCamera;

	Transform& camTransform = mReflectionCamera->transform;

	// invert the pitch of the camera
	auto camUp = camTransform.up();
	auto up = glm::vec3{ 0, 1, 0 };
	auto cosUp = glm::dot(up, camUp);
	auto angle = glm::acos(cosUp);
	float sign = glm::sign(glm::dot(glm::cross(camUp, up), camTransform.right()));
	camTransform.rotateBy(glm::normalize(glm::angleAxis(sign * 2 * angle, camTransform.right())));

	// put camera under water
	float offset = camTransform.getPosition().y - mWaterY;
	camTransform.moveBy(glm::vec3{ 0, - 2 * offset, 0 });

	// render to reflection target
	Engine::renderSys.deferredRenderingFBO = mReflectionFbo;
	Engine::renderSys.setClipPlane(glm::vec4{ 0, 1, 0, -mWaterY });
	Engine::renderSys.renderScene(&mReflectionTarget, RenderPhase::WATER);

	Engine::renderSys.camera = oldCamera;
}


void WaterMaterial::renderRefraction()
{
	Engine::renderSys.deferredRenderingFBO = mRefractionFbo;
	Engine::renderSys.setClipPlane(glm::vec4{ 0, -1, 0, mWaterY });
	Engine::renderSys.renderScene(&mRefractionTarget, RenderPhase::WATER);
}

void WaterMaterial::use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mReflectionTarget.getColorBuffer().getId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mRefractionTarget.getColorBuffer().getId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mDuDvMap.getId());
	shader.use();
	shader.setFloat(mMoveDuDvLocation, mMoveDuDv);
}

void WaterMaterial::after()
{
	
}