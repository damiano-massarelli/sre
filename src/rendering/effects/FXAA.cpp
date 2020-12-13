#include "rendering/effects/FXAA.h"
#include "Engine.h"
#include "cameras/CameraComponent.h"
#include "rendering/effects/GammaCorrection.h"
#include <glm/glm.hpp>

FXAA::FXAA()
    : Effect{ "fxaa", "effects/fxaa.glsl" } { }

void FXAA::onSetup(Shader& postProcessingShader) {

#ifdef SRE_DEBUG
    // Make sure FXAA is applied after GammaCorrection
    assert(Engine::renderSys.effectManager.hasEffect<GammaCorrection>());

    // Make sure bilinear filtering is enabled on the FXAA input texture
    assert(Engine::renderSys.lightPassTarget.getSettings().appearanceOptions.magFilter == GL_LINEAR);
#endif  // SSR_DEBUG

    mPostProcessingShader = postProcessingShader;
}
