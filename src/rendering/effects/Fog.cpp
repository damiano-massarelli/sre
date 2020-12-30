#include "rendering/effects/Fog.h"
#include "cameras/CameraComponent.h"


#include "Engine.h"

Fog::Fog()
    : Effect{ "fog", "effects/fog.glsl" } {
}

void Fog::onSetup(Shader& postProcessingShader) {
    mPostProcessingShader = postProcessingShader;

    mPostProcessingShader.setVec3("uFogColor", mColor);
    mPostProcessingShader.setVec2("uFogParams", glm::vec2{mDensity, mRapidity});
}

Fog::~Fog() {}

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
