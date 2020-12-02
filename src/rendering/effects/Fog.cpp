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

void Fog::update(Shader& postProcessingShader) {
    ShaderScopedUsage useShader{ postProcessingShader };

    const RenderSystem& renderSystem = Engine::renderSys;
    const auto cameraComponent = renderSystem.getCamera()->getComponent<CameraComponent>();
    const float near = cameraComponent->getNearPlaneDistance();
    const float far = cameraComponent->getFarPlaneDistance();

    if (near != mNearPlane) {
        mNearPlane = near;
        postProcessingShader.setFloat("_fog_nearPlane", mNearPlane);
    }
    if (far != mFarPlane) {
        mFarPlane = far;
        postProcessingShader.setFloat("_fog_farPlane", mFarPlane);
    }
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
