#pragma once
#include "Effect.h"
#include "rendering/effects/GaussianBlur.h"
#include "rendering/materials/Shader.h"
#include <cstdint>
#include <vector>

class SSR : public Effect {
private:
    int mPositionTexture = -1;
    int mNormalTexture = -1;
    int mMaterialTexture = -1;
    int mDiffuseTexture = -1;
    int mFallbackSkyboxTexture = -1;
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

    Texture mFallbackSkybox;

    Shader mPostProcessingShader;

    // vector of render targets referring to different mipmaps levels of
    // the light pass target texture.
    std::vector<RenderTarget> mBlurRenderTargets;
    std::vector<GaussianBlur> mGaussianBlurEffects;

public:
    SSR();

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;

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

    ~SSR();
};
