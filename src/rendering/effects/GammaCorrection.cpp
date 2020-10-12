#include "rendering/effects/GammaCorrection.h"

GammaCorrection::GammaCorrection()
    : Effect{ "gammaCorrection", "effects/gammaCorrection.glsl" } { }

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
        ShaderScopedUsage useShader{ postProcessingShader };
        postProcessingShader.setFloat("_gc_gamma", mGamma);
        postProcessingShader.setFloat("_gc_exposure", mExposure);
        mNeedUpdate = false;
    }
}
