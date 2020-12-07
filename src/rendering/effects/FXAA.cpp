#include "rendering/effects/FXAA.h"
#include "Engine.h"
#include "cameras/CameraComponent.h"
#include "rendering/effects/GammaCorrection.h"
#include <glm/glm.hpp>

FXAA::FXAA()
    : Effect{ "fxaa", "effects/fxaa.glsl" } { }

void FXAA::setReduceMultiplier(float multiplier) {
    mReduceMultiplier = multiplier;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setFloat("uFXAAReduceMultiplier", multiplier);
}

void FXAA::setMinReduceMultiplier(float minReduce) {
    mMinReduce = minReduce;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setFloat("uFXAAReduceMin", minReduce);
}

void FXAA::setMaxSpan(float span) {
    mMaxSpan = span;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setFloat("uFXAAMaxSpan", span);
}

void FXAA::setLumaThreshold(float threshold) {
    mLumaThreshold = threshold;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setFloat("uFXAALumaThreshold", threshold);
}

void FXAA::onSetup(Shader& postProcessingShader) {

#ifdef SRE_DEBUG
    // Make sure FXAA is applied after GammaCorrection
    assert(Engine::renderSys.effectManager.hasEffect<GammaCorrection>());
#endif  // SSR_DEBUG

    mPostProcessingShader = postProcessingShader;

    setReduceMultiplier(mReduceMultiplier);
    setMinReduceMultiplier(mMinReduce);
    setMaxSpan(mMaxSpan);
    setLumaThreshold(mLumaThreshold);
}
