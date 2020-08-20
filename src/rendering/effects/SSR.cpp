#include "SSR.h"
#include "Engine.h"
#include "rendering/RenderSystem.h"
#include "rendering/effects/EffectManager.h"
#include <cassert>

SSR::SSR() : Effect{"ssr", "effects/ssr.glsl"} {
	mPositionTexture = Engine::renderSys.effectManager.getTexture();
	mNormalTexture = Engine::renderSys.effectManager.getTexture();
	mSpecularTexture = Engine::renderSys.effectManager.getTexture();
	assert(mPositionTexture != -1);
	assert(mNormalTexture != -1);
	assert(mSpecularTexture != -1);
}

void SSR::onSetup(Shader& postProcessingShader) {
	//ShaderScopedUsage useShader{ postProcessingShader };

	mPostProcessingShader = postProcessingShader;

	mPostProcessingShader.setInt("_ssr_position", mPositionTexture);
	mPostProcessingShader.setInt("_ssr_normals", mNormalTexture);
	mPostProcessingShader.setInt("_ssr_specular", mSpecularTexture);

	// Parameter defaults
	mPostProcessingShader.setFloat("_ssr_ray_max_distance", mMaxDistance);
	mPostProcessingShader.setFloat("_ssr_ray_resolution", mResolution);
	mPostProcessingShader.setInt("_ssr_ray_steps", mSteps);
	mPostProcessingShader.setFloat("_ssr_ray_hit_threshold", mHitThreshold);
}

void SSR::update(Shader& postProcessingShader) {
	ShaderScopedUsage useShader{ postProcessingShader };

	const RenderSystem& renderSystem = Engine::renderSys;
	const glm::mat4 currentProjectViewMatrix = renderSystem.getProjectionMatrix() * renderSystem.getViewMatrix(renderSystem.getCamera()->transform);

	// TODO optimize me
	postProcessingShader.setVec3("_ssr_cameraPosition", renderSystem.getCamera()->transform.getPosition());
	postProcessingShader.setVec3("_ssr_cameraDirection", renderSystem.getCamera()->transform.forward());
	postProcessingShader.setMat4("_ssr_projectionView", currentProjectViewMatrix);

	glActiveTexture(GL_TEXTURE0 + mPositionTexture);
	glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getPositionBuffer().getId());

	glActiveTexture(GL_TEXTURE0 + mNormalTexture);
	glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getNormalBuffer().getId());

	glActiveTexture(GL_TEXTURE0 + mSpecularTexture);
	glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getAdditionalBuffer().getId());
}

void SSR::setMaxDistance(float maxDistance) {
	if (mMaxDistance != maxDistance) {
		mMaxDistance = maxDistance;

		ShaderScopedUsage useShader{ mPostProcessingShader };
		mPostProcessingShader.setFloat("_ssr_ray_max_distance", mMaxDistance);
	}	
}

void SSR::setResolution(float resolution) {
	if (mResolution != resolution) {
		mResolution = resolution;

		ShaderScopedUsage useShader{ mPostProcessingShader };
		mPostProcessingShader.setFloat("_ssr_ray_resolution", mResolution);
	}
}

void SSR::setSteps(int steps) {
	if (mSteps != steps) {
		mSteps = steps;

		ShaderScopedUsage useShader{ mPostProcessingShader };
		mPostProcessingShader.setInt("_ssr_ray_steps", mSteps);
	}
}

void SSR::setHitThreshold(float hitThreshold) {
	if (mHitThreshold != hitThreshold) {
		mHitThreshold = hitThreshold;

		ShaderScopedUsage useShader{ mPostProcessingShader };
		mPostProcessingShader.setFloat("_ssr_ray_hit_threshold", mHitThreshold);
	}
}

SSR::~SSR() {
	Engine::renderSys.effectManager.releaseTexture(mPositionTexture);
	Engine::renderSys.effectManager.releaseTexture(mNormalTexture);
	Engine::renderSys.effectManager.releaseTexture(mSpecularTexture);
}
