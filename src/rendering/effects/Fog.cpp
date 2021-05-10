#include "rendering/effects/Fog.h"
#include "Engine.h"

Fog::Fog()
    : Effect{ "fog", "effects/fog.glsl" } {
    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setInt("uInputTexture", 0);
    mPostProcessingShader.setInt("uDepthTexture", 1);
    mPostProcessingShader.setVec3("uFogColor", mColor);
    mPostProcessingShader.setVec2("uFogParams", glm::vec2{ mDensity, mRapidity });
    mPostProcessingShader.bindUniformBlock("Camera", Engine::renderSys.CAMERA_UNIFORM_BLOCK_INDEX);
}

void Fog::setColor(const glm::vec3& color) {
    mColor = color;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setVec3("uFogColor", mColor);
}

void Fog::setDensity(float distance) {
    if (distance != mDensity) {
        mDensity = distance;

        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setVec2("uFogParams", glm::vec2{ mDensity, mRapidity });
    }
}

void Fog::setRapidity(float rapidity) {
    if (rapidity != mRapidity) {
        mRapidity = rapidity;

        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setVec2("uFogParams", glm::vec2{ mDensity, mRapidity });
    }
}

void Fog::applyEffect(const Texture& input, const RenderTarget* dst) {
    Effect::applyEffect({ input, Engine::renderSys.gBuffer.getDepthBuffer() }, dst);
}
