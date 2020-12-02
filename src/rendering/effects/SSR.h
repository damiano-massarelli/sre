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
    std::int32_t mCameraPositionLocation = -1;
    std::int32_t mCameraDirectionLocation = -1;
    std::int32_t mProjectionViewLocation = -1;
    std::int32_t mFrustumPlanesLocation = -1;

    float mMaxDistance = 15.f;
    std::int32_t mNumSamples = 100;
    std::int32_t mSteps = 5;
    float mHitThreshold = 0.5f;
    float mNearPlane = 0.f;
    float mFarPlane = 0.f;

    Shader mPostProcessingShader;

    GaussianBlur mGaussianBlur;

    // vector of render targets referring to different mipmaps levels of
    // the light pass target texture.
    std::vector<RenderTarget> mBlurRenderTargets;
    std::vector<GaussianBlur> mGaussianBlurEffects;

public:
    SSR();

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;

    void setMaxDistance(float maxDistance);
    void setNumSamples(std::int32_t numSamples);
    void setSteps(std::int32_t steps);
    void setHitThreshold(float hitThreshold);

    float getMaxDistance() const { return mMaxDistance; }
    std::int32_t getNumSamples() const { return mNumSamples; }
    std::int32_t getSteps() const { return mSteps; }
    float geHitThreshold() const { return mHitThreshold; }

    ~SSR();
};
