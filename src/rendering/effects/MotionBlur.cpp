#include "rendering/effects/MotionBlur.h"
#include "Engine.h"

MotionBlur::MotionBlur()
    : Effect{ "motionBlur", "effects/motionBlur.glsl" } {
    
    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPrevProjViewMatrixLocation = mPostProcessingShader.getLocationOf("_mb_prevProjView");
    mCurrentProjViewMatrixLocation = mPostProcessingShader.getLocationOf("_mb_currProjView");
    mPostProcessingShader.setInt("inputTexture", 0);
    mPostProcessingShader.setInt("positionTexture", 1);
}

void MotionBlur::setBlurFactor(float blurFactor) {
    mBlurFactor = blurFactor;
    mBlurNeedsUpdate = true;
}

void MotionBlur::onSetup(Shader& postProcessingShader) {
    const RenderSystem& rsys = Engine::renderSys;
    mPrevProjViewMatrix = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.getCamera()->transform);
}

void MotionBlur::update(Shader& postProcessingShader) {
    ShaderScopedUsage useShader{ mPostProcessingShader };
    if (mBlurNeedsUpdate) {
        mBlurNeedsUpdate = false;
        mPostProcessingShader.setFloat("_mb_blurFactor", mBlurFactor);
    }

    RenderSystem& rsys = Engine::renderSys;

    glm::mat4 currProjViewMat = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.getCamera()->transform);
    mPostProcessingShader.setMat4(mPrevProjViewMatrixLocation, mPrevProjViewMatrix);
    mPostProcessingShader.setMat4(mCurrentProjViewMatrixLocation, currProjViewMat);

    mPrevProjViewMatrix = currProjViewMat;
}

void MotionBlur::applyEffect(const Texture& input, const RenderTarget* dst) {
    Effect::applyEffect({ input, Engine::renderSys.gBuffer.getPositionBuffer() }, dst);
}
