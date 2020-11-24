#include "rendering/light/DirectionalLight.h"
#include "Engine.h"
#include "rendering/shadow/ShadowMappingSettings.h"

DirectionalLight::DirectionalLight(const GameObjectEH& go)
    : Light{ go, Light::Type::DIRECTIONAL } { }

const RenderTarget& DirectionalLight::getShadowMapTarget() const {
    return mShadowMapTarget;
}

void DirectionalLight::setCastShadowMode(Light::ShadowCasterMode mode) {
    Light::setCastShadowMode(mode);

    if (mode == Light::ShadowCasterMode::NO_SHADOWS)
        return;

    ShadowMappingSettings& settings = Engine::renderSys.shadowMappingSettings;
    mShadowTexture = Texture::load(nullptr, settings.mapWidth, settings.mapHeight, GBuffer::DEPTH_BUFFER_SETTINGS);
    mShadowMapTarget = RenderTarget{ nullptr, &mShadowTexture };
}
