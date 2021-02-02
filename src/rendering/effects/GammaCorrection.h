#pragma once
#include "rendering/effects/Effect.h"

/**
 * Performs gamma correction and tone mapping.
 * This effect should always be the last in the
 * effect pipeline.
 */
class GammaCorrection : public Effect {
private:
    bool mNeedUpdate = false;
    float mGamma = 2.2f;
    float mExposure = 1.0f;

    static constexpr Texture::Settings OUTPUT_TEXTURE_SETTINGS{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, { false, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE } };

public:
    GammaCorrection();

    /**
     * Sets the gamma value for gamma correction.
     * @param gamma the gamma value to use.
     */
    void setGamma(float gamma);

    /**
     * Sets the exposure used in tone mapping.
     * @param exposure the exposure to use.
     */
    void setExposure(float exposure);

    void update() override;
};
