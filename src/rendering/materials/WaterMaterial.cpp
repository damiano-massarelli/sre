#include "WaterMaterial.h"
#include "RenderSystem.h"
#include "Engine.h"
#include "RenderPhase.h"
#include <glm/glm.hpp>
#include <cmath>

WaterMaterial::WaterMaterial(float waterY, const Texture& dudv, const Texture& normalMap)
	: Material{"shaders/waterVS.glsl", "shaders/waterFS.glsl"}, mWaterY{ waterY }, mDuDvMap{ dudv },
	mNormalMap { normalMap }
{
	mReflectionTarget.create(320, 180);
	mReflectionFbo.init(320, 180);

	mRefractionTarget.create(1280, 720);
	mRefractionFbo.init(1280, 720);

	mReflectionCamera = Engine::gameObjectManager.createGameObject();

	// don't render when rendering for water or shadows
	unSupportedRenderPhases |= RenderPhase::ALL & ~RenderPhase::DEFERRED_RENDERING;

	mEventCrumb = Engine::eventManager.addListenerFor(EventManager::PRE_RENDER_EVENT, this, true);
	shader.use();
	shader.setInt("reflection", 0);
	shader.setInt("refraction", 1);
	shader.setInt("dudvMap", 2);
	shader.setInt("normalMap", 3);
	shader.setInt("depthMap", 4);
	shader.setInt("groundDiffuseMap", 5);
	shader.setInt("groundSpecularMap", 6);
	shader.setInt("groundNormalMap", 7);

	shader.setFloat("near", Engine::renderSys.getNearPlane());
	shader.setFloat("far", Engine::renderSys.getFarPlane());

	shader.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);
	shader.bindUniformBlock("Camera", RenderSystem::CAMERA_UNIFORM_BLOCK_INDEX);

	mMoveDuDvLocation = shader.getLocationOf("moveDuDv");
}

void WaterMaterial::onEvent(SDL_Event e)
{
	float delta = (*(static_cast<float*>(e.user.data1)));
	mMoveDuDv += waveSpeed * delta / 1000.0f;
	mMoveDuDv = std::fmod(mMoveDuDv, 1.0f);

	auto& renderSys = Engine::renderSys;

	float oldShadowStrength = renderSys.shadowMappingSettings.getShadowStrength();
	renderSys.shadowMappingSettings.setShadowStrength(0.0f);

	auto oldDeferredFbo = renderSys.deferredRenderingFBO;
	renderSys.enableClipPlane();

	renderRefraction();

	renderReflection();

	renderSys.shadowMappingSettings.setShadowStrength(oldShadowStrength);

	renderSys.disableClipPlane();
	renderSys.deferredRenderingFBO = oldDeferredFbo;
}


std::size_t WaterMaterial::hash() const
{
	return Material::hash()
		+ mDuDvMap.getId()
		+ mNormalMap.getId();
}


bool WaterMaterial::equalsTo(const Material* rhs) const
{
	if (shader.getId() != rhs->shader.getId())
		return false;

	auto other = static_cast<const WaterMaterial*>(rhs);

	return Material::equalsTo(rhs)
		&& mDuDvMap.getId() == other->mDuDvMap.getId()
		&& mNormalMap.getId() == other->mNormalMap.getId()
		&& waveSpeed == other->waveSpeed
		&& mWaterY == other->mWaterY;
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
	Engine::renderSys.setClipPlane(glm::vec4{ 0, -1, 0, mWaterY + 1 });
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

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mNormalMap.getId());

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mRefractionTarget.getDepthBuffer().getId());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mRefractionFbo.getDiffuseBuffer().getId());

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, mRefractionFbo.getAdditionalBuffer().getId());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, mRefractionFbo.getNormalBuffer().getId());

	shader.use();
	shader.setFloat(mMoveDuDvLocation, mMoveDuDv);
}

void WaterMaterial::after()
{
	for (int i = 0; i <= 7; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}