#pragma once

#include "rendering/effects/Effect.h"
#include "rendering/materials/Texture.h"
#include "rendering/RenderTarget.h"

/**
 * Fast Approximate Anti-Aliasing Effect.
 * FXAA must always be applied after GammaCorrection.
 */
class FXAA : public Effect {
public:
    FXAA();

    void applyEffect(const Texture& input, const RenderTarget* dst) override;
};
