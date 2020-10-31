#pragma once
#include "gameobject/GameObjectEH.h"
#include "rendering/RenderTarget.h"
#include "rendering/effects/Effect.h"
#include "rendering/materials/Shader.h"

#include "rendering/effects/GaussianBlur.h"

/**
 * Good rays effect.
 */
class GodRays : public Effect {
private:
    Texture mOcclusionTexture;
    RenderTarget mOcclusionTarget;

    int mBlurredTexture;

    Shader mOcclusionCreator;

    std::int32_t mLightScreenPosLocation = -1;
    std::int32_t mRadiusLocation = -1;

    std::int32_t mLightScreenPosForRadialBlurLocation = -1;

    bool mNeedUpdate = false;
    float mDensity = 1.0;
    float mDecayRatio = 0.95f;
    float mWeight = 0.9f;

public:
    /** The light emitting god rays */
    GameObjectEH light;

    /** The light casting god rays is represented by a sphere, this is its
     * radius */
    float lightRadius = 0.5f;

    /**
     * Creates a new GodRays effect.
     * The scale factor determines the size (with respect to the resolution of
     * the screen) of the texture on which the effect created. A scaleFactor
     * near 1.0 will create a good quality effect but it may impact
     * performances.
     * @param scaleFactor the scale factor to use (default is 0.7f).
     */
    GodRays(float scaleFactor = 0.7f);

    /**
     * Sets the density of god rays.
     * Density controls the span of rays.
     * @param density the density to set
     */
    void setDensity(float density);

    /**
     * @return the density of god rays
     */
    float getDensity() const;

    /**
     * Sets the decay ration of god rays.
     * The decay ratio controls how fast rays fade into black.
     * @param decayRatio the decay ratio
     */
    void setDecayRatio(float ratio);

    /**
     * @return the decay ratio
     */
    float getDecayRatio() const;

    /**
     * Sets the weight of god rays.
     * The weight controls how bright god rays are.
     * @param weigh the weight to set.
     */
    void setWeight(float weight);

    /**
     * @return the weight of god rays
     */
    float getWeight() const;

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;

    virtual ~GodRays();
};
