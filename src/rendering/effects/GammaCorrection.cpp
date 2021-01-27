#include "rendering/effects/GammaCorrection.h"

GammaCorrection::GammaCorrection()
    : Effect{ "gammaCorrection", "effects/gammaCorrection.glsl" } {

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setFloat("_gc_gamma", mGamma);
    mPostProcessingShader.setFloat("_gc_exposure", mExposure);
}

void GammaCorrection::setGamma(float gamma) {
    mGamma = gamma;
    mNeedUpdate = true;
}

void GammaCorrection::setExposure(float exposure) {
    mExposure = exposure;
    mNeedUpdate = true;
}

void GammaCorrection::update(Shader& postProcessingShader) {
    if (mNeedUpdate) {
        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setFloat("_gc_gamma", mGamma);
        mPostProcessingShader.setFloat("_gc_exposure", mExposure);
        mNeedUpdate = false;
    }
}
