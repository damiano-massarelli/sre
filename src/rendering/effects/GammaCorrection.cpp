#include "rendering/effects/GammaCorrection.h"
#include "Engine.h"

GammaCorrection::GammaCorrection()
    : Effect{ { "gc",
        "effects/gammaCorrection.glsl",
        { Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight() },
        OUTPUT_TEXTURE_SETTINGS } } {

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

void GammaCorrection::update() {
    if (mNeedUpdate) {
        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setFloat("_gc_gamma", mGamma);
        mPostProcessingShader.setFloat("_gc_exposure", mExposure);
        mNeedUpdate = false;
    }
}
