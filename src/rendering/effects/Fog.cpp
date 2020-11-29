#include "rendering/effects/Fog.h"

#include "Engine.h"

Fog::Fog()
    : Effect{ "fog", "effects/fog.glsl" } {
}

void Fog::onSetup(Shader& postProcessingShader) {
    mPostProcessingShader = postProcessingShader;

    mPostProcessingShader.setVec3("uFogColor", glm::vec3{0.f, 0.f, 0.1f});
    mPostProcessingShader.setVec2("uFogParams", glm::vec2{15.f, 2.f});
}

void Fog::update(Shader& postProcessingShader) {
}

Fog::~Fog() {
}

void Fog::setColor(const glm::vec3& color) {
    mColor = color;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setVec3("uFogColor", mColor);
}

void Fog::setStartDistance(float distance) {
    mStartDistance = distance;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setVec2("uFogParams", glm::vec2{ mStartDistance, mRapidity });
}

void Fog::setRapidity(float rapidity) {
    mRapidity = rapidity;

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setVec2("uFogParams", glm::vec2{ mStartDistance, mRapidity });
}
