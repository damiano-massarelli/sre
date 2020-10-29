#pragma once
#include "rendering/RenderTarget.h"
#include "rendering/light/Light.h"

class DirectionalLight : public Light {
private:
    Texture mShadowTexture;
    RenderTarget mShadowMapTarget;

public:
    DirectionalLight(const GameObjectEH& go);

    const RenderTarget& getShadowMapTarget() const;

    virtual void setCastShadowMode(Light::ShadowCasterMode mode) override;

    virtual ~DirectionalLight() = default;
};
