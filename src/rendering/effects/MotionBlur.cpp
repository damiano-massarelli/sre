#include "rendering/effects/MotionBlur.h"
#include "Engine.h"

MotionBlur::MotionBlur()
	: Effect{"motionBlur", "effects/motionBlur.glsl"}
{
	RenderSystem& rsys = Engine::renderSys;
	mPrevProjViewMatrix = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.getCamera()->transform);

	mPositionTexture = Engine::renderSys.effectManager.getTexture();
}

void MotionBlur::setBlurFactor(float blurFactor)
{
	mBlurFactor = blurFactor;
	mBlurNeedsUpdate = true;
}

void MotionBlur::onSetup(Shader& postProcessingShader)
{
	mPrevProjViewMatrixLocation = postProcessingShader.getLocationOf("_mb_prevProjView");
	mCurrentProjViewMatrixLocation = postProcessingShader.getLocationOf("_mb_currProjView");
	postProcessingShader.setInt("_mb_positionTexture", mPositionTexture);
}


void MotionBlur::update(Shader& postProcessingShader)
{
	ShaderScopedUsage useShader{ postProcessingShader };
	if (mBlurNeedsUpdate) {
		mBlurNeedsUpdate = false;
		postProcessingShader.setFloat("_mb_blurFactor", mBlurFactor);
	}

	RenderSystem& rsys = Engine::renderSys;

	glm::mat4 currProjViewMat = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.getCamera()->transform);
	postProcessingShader.setMat4(mPrevProjViewMatrixLocation, mPrevProjViewMatrix);
	postProcessingShader.setMat4(mCurrentProjViewMatrixLocation, currProjViewMat);
	glActiveTexture(GL_TEXTURE0 + mPositionTexture);
	glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getPositionBuffer().getId());

	mPrevProjViewMatrix = currProjViewMat;
}

MotionBlur::~MotionBlur()
{
	Engine::renderSys.effectManager.releaseTexture(mPositionTexture);
}