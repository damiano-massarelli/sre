#include "MotionBlur.h"
#include "Engine.h"

MotionBlur::MotionBlur()
	: Effect{"motionBlur", "effects/motionBlur.glsl"}
{
	RenderSystem& rsys = Engine::renderSys;
	mPrevProjViewMatrix = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.camera->transform);

	mPositionTexture = Engine::renderSys.effectManager.getTexture();
}

void MotionBlur::setBlurFactor(float blurFactor)
{
	mBlurFactor = blurFactor;
	mBlurNeedsUpdate = true;
}

void MotionBlur::onSetup(Shader& postProcessingShader)
{
	mPrevProjViewMatrixLocation = postProcessingShader.getLocationOf("_mbPrevProjView");
	postProcessingShader.setInt("_mbPositionTexture", mPositionTexture);
}

void MotionBlur::update(Shader& postProcessingShader)
{
	postProcessingShader.use();

	if (mBlurNeedsUpdate) {
		mBlurNeedsUpdate = false;
		postProcessingShader.setFloat("_mbBlurFactor", mBlurFactor);
	}

	postProcessingShader.setMat4(mPrevProjViewMatrixLocation, mPrevProjViewMatrix);
	glActiveTexture(GL_TEXTURE0 + mPositionTexture);
	glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getPositionBuffer().getId());

	RenderSystem& rsys = Engine::renderSys;
	mPrevProjViewMatrix = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.camera->transform);
}

MotionBlur::~MotionBlur()
{
	Engine::renderSys.effectManager.releaseTexture(mPositionTexture);
}