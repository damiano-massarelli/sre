#pragma once
#include "Effect.h"
#include "rendering/materials/Shader.h"
#include "rendering/effects/GaussianBlur.h"
#include <cstdint>

class SSR : public Effect {
private:
    int mPositionTexture = -1;
    int mNormalTexture = -1;
    int mMaterialTexture = -1;
    int mBlurredSceneTexture = -1;
    std::int32_t mCameraPositionLocation = -1;
    std::int32_t mCameraDirectionLocation = -1;
    std::int32_t mProjectionViewLocation = -1;
    std::int32_t mFrustumPlanesLocation = -1;

    float mMaxDistance = 15.f;
    float mResolution = 0.5f;  // 1 to procede every pixel, within 0 and 1 to sample further
    std::int32_t mSteps = 5;
    float mHitThreshold = 0.5f;
    float mNearPlane = 0.f;
    float mFarPlane = 0.f;

    Shader mPostProcessingShader;

    GaussianBlur mGaussianBlur;

public:
    SSR();

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;

    void setMaxDistance(float maxDistance);
    void setResolution(float resolution);
    void setSteps(std::int32_t steps);
    void setHitThreshold(float hitThreshold);

    float getMaxDistance() const { return mMaxDistance; }
    float getResolution() const { return mResolution; }
    std::int32_t getSteps() const { return mSteps; }
    float geHitThreshold() const { return mHitThreshold; }

    ~SSR();
};
