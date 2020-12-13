#pragma once

#include "rendering/effects/Effect.h"

/**
 * Fast Approximate Anti-Aliasing Effect.
 * FXAA must always be applied after GammaCorrection.
 */
class FXAA : public Effect {
private:
    Shader mPostProcessingShader;

public:
    FXAA();

    virtual void onSetup(Shader& postProcessingShader) override;
};
