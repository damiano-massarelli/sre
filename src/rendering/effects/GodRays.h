#pragma once
#include "gameobject/GameObjectEH.h"
#include "rendering/effects/Effect.h"
#include "rendering/effects/GaussianBlur.h"
#include <cstdint>
#include <memory>

/**
 * Good rays effect.
 */
class GodRays : public Effect {
private:
    std::int32_t mLightScreenPosLocation = -1;
    std::int32_t mRadiusLocation = -1;
    std::int32_t mLightColorLocation = -1;

    bool mNeedUpdate = true;
    float mDensity = 1.0;
    float mDecayRatio = 0.96f;
    float mWeight = 0.1f;
    std::int32_t mNumSamples = 100;

    // The light emitting god rays
    GameObjectEH mLight;
    // The light component of the attached GameObject
    std::weak_ptr<class Light> mLightComponent;

public:
    /** The light casting god rays is represented by a sphere, this is its
     * radius */
    float lightRadius = 10.0f;

    /** The light casting god rays is represented by a sphere, this parameter
     * determines whether the sphere is scaled depending on its distance from the camera. */
    bool isPerspective = true;

    /**
     * Creates a new GodRays effect.
     */
    GodRays();

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
     * @param weight the weight to set.
     */
    void setWeight(float weight);

    /**
     * @return the weight of god rays
     */
    float getWeight() const;

    /**
     * Sets the number of samples used to create the god rays radial blur effect.
     * @param samples the number of samples.
     */
    void setNumSamples(std::int32_t samples);

    /**
     * @return the number of samples used to create the god rays radial blur effect
     */
    std::int32_t getNumSamples() const;

    /**
     * Sets the light for which god rays are generated.
     * The given GameObject must have a Light component.
     * @param light the light for which god rays are generated.
     */
    void setLight(const GameObjectEH& light);

    /**
     * @return the light for which god rays are generated
     */
    const GameObjectEH& getLight() const;

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;
};
