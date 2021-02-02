#pragma once
#include "rendering/RenderTarget.h"
#include "rendering/effects/Effect.h"
#include "rendering/effects/GaussianBlur.h"
#include "rendering/materials/Texture.h"

/**
 * Bloom effect.
 * Areas directly hit by light will look brighter.
 */
class Bloom : public Effect {
private:
    Shader mBloomExtractor;
    RenderTarget mTarget;
    Texture mBloom;
    GaussianBlur mGaussianBlur;

    float mBloomFactor = 0.7f;

public:
    /**
     * Creates a new bloom effect.
     * The scaling factor controls the resolution of the blurring stage
     * (performed to create bloom). A value of 1 means that the blurring stage
     * will use a texture that has the same resolution as the current window.
     * This will result in a very precise bloom effect but the overall bloom
     * won't be very noticeable. A lower value (say 0.75f) will create a bloom
     * effect that may have some flickering but it will be more noticeable and
     * faster. To reduce or amplify the effect of bloom setBloomFactor can also
     * be used.
     * @param scaleFactor the scale of the blurring stage used for bloom.
     */
    Bloom(float scaleFactor = 0.7f);

    void applyEffect(const Texture& input, const RenderTarget* dst = nullptr) override;

    /**
     * Sets the impact of the bloom effect on the final image.
     * A value of 1 means the bloom effect fully applied.
     * A value of 0 means the bloom effect is not applied.
     */
    void setBloomFactor(float bloomFactor);

    float getBloomFactor() const;

    ~Bloom();
};
