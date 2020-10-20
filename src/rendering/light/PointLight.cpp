#include "rendering/light/PointLight.h"
#include "rendering/materials/Texture.h"
#include <algorithm>

PointLight::PointLight(const GameObjectEH& go)
    : Light{ go, Light::Type::POINT } { }

float PointLight::getRadius() const {
    float linear = attenuationLinear;
    float constant = attenuationConstant;
    float quadratic = attenuationQuadratic;
    float lightMax = std::max(
        { diffuseColor.r, diffuseColor.g, diffuseColor.b, specularColor.r, specularColor.g, specularColor.b });
    float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * lightMax)))
        / (2 * quadratic);

    return radius;
}

void PointLight::setCastShadowMode(ShadowCasterMode mode) {
    Light::setCastShadowMode(mode);

    mPointShadowTarget = RenderTarget{};

    if (mode == ShadowCasterMode::NO_SHADOWS)
        return;

     mPointShadowCubeTexture = Texture::loadCubemap({ { "front", nullptr },
                                            { "back", nullptr },
                                            { "top", nullptr },
                                            { "bottom", nullptr },
                                            { "left", nullptr },
                                            { "right", nullptr } },
        1024,
        1024,
        GL_CLAMP_TO_EDGE,
        GL_CLAMP_TO_EDGE,
        GL_CLAMP_TO_EDGE,
        GL_DEPTH_STENCIL,
        GL_UNSIGNED_INT_24_8,
        GL_DEPTH24_STENCIL8);

    mPointShadowTarget = RenderTarget{ nullptr, &mPointShadowCubeTexture };
}

const RenderTarget& PointLight::getPointShadowTarget() const { return mPointShadowTarget; }
