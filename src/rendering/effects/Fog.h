#pragma once

#include "rendering/RenderTarget.h"
#include "rendering/effects/Effect.h"
#include "rendering/materials/Shader.h"
#include "rendering/materials/Texture.h"

/**
 *  Fog effect.
 */
class Fog : public Effect {
private:
    glm::vec3 mColor = { 0.1f, 0.1f, 0.1f };

    // Reasonable values are between 0.0001 and 0.1
    float mDensity = 0.0035f;

    // Exponent of the fog, reasonable values are between 1.0 and 15.0
    float mRapidity = 3.f;

public:
    Fog();

    // Setters
    void setColor(const glm::vec3& color);
    void setDensity(float distance);
    void setRapidity(float rapidity);

    // Getters
    glm::vec3 getColor() const { return mColor; }
    float getDensity() const { return mDensity; }
    float getRapidity() const { return mRapidity; }

    void applyEffect(const Texture& input, const RenderTarget* dst = nullptr) override;
};
