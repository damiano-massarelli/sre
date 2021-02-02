#pragma once
#include "Effect.h"
#include "rendering/effects/GaussianBlur.h"
#include "rendering/materials/Shader.h"
#include <cstdint>
#include <vector>

class SSR : public Effect {
private:
    std::int32_t mCameraPositionLocation = -1;
    std::int32_t mProjectionViewLocation = -1;
    std::int32_t mFrustumPlanesLocation = -1;

    float mMaxReflectionDistance = 15.F;
    std::int32_t mNumSamples = 100;
    std::int32_t mSteps = 5;
    float mHitThreshold = 0.5F;
    float mSteepAngleHitThresholdMultiplier = 8.F;
    float mNearPlane = 0.F;
    float mFarPlane = 0.F;

    Texture mSSROutput;
    Texture mFallbackSkybox;

    Shader mSSRExtract;

    // vector of render targets referring to different mipmaps levels of
    // ssr texture
    std::vector<RenderTarget> mSSROutpuRenderTargets;
    std::vector<GaussianBlur> mGaussianBlurEffects;

    void createAndSetupExtractShader();

public:
    SSR();

    void update() override;
    void applyEffect(const Texture& input, const RenderTarget* dst) override;

    void setMaxReflectionDistance(float maxReflectionDistance);
    void setNumSamples(std::int32_t numSamples);
    void setSteps(std::int32_t steps);
    void setHitThreshold(float hitThreshold);
    void setSteepAngleHitThresholdMultiplier(float multiplier);
    void setFallbackSkyboxTexture(Texture fallbackSkybox);

    float getMaxReflectionDistance() const { return mMaxReflectionDistance; }
    std::int32_t getNumSamples() const { return mNumSamples; }
    std::int32_t getSteps() const { return mSteps; }
    float geHitThreshold() const { return mHitThreshold; }
    float getSteepAngleHitThresholdMultiplier() const { return mSteepAngleHitThresholdMultiplier; }
    Texture getFallbackSkyboxTexture() const { return mFallbackSkybox; };
};
