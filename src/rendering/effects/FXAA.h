#pragma once

#include "rendering/effects/Effect.h"

/**
 * Fast Approximate Anti-Aliasing Effect.
 * FXAA must always be applied after GammaCorrection.
 */
class FXAA : public Effect {
private:
    float mReduceMultiplier = 1 / 8.F;

    float mMinReduce = 1 / 128.F;

    float mMaxSpan = 8.F;

    float mLumaThreshold = 0.5F;

    Shader mPostProcessingShader;

public:
    FXAA();

    /**
     * Sets how much the FXAA effect is reduced or emphasised.
     * The FXAA effect is, by itself, reduced for brighter pixels. However, this multiplier
     * can further reduce the anti-aliasing effect or emphasise it.
     * If the provided multiplier is greater than 1 the effect of the anti-aliasing filter is reduced.
     * When the provided multiplier is less than 1 the effect of the anti-aliasing filter is emphasised.
     * @param multiplier a multiplier to reduce or emphasise the anti-aliasing effect.
     */
    void setReduceMultiplier(float multiplier);

    /**
     * Sets the minimum value for the reduce multiplier.
     * This ensures that a certain amount of reduction (it the provided parameter is greater than 1)
     * or emphasis (if the provided parameter is less the 1) is always performed.
     * getReduceMultiplier can still return values that are smaller than the provided parameter.
     * This is because the reduceMultiplier also takes into account the brighness of the pixels the effect is processing.
     * @param minReduce the minimum value for the reduce multiplier.
     */
    void setMinReduceMultiplier(float minReduce);

    /**
     * Sets the max span used when blurring edges.
     * @param span the max span used to blur edges.
     */
    void setMaxSpan(float span);

    /**
     * Sets the threshold used to detect an edge based on the luma range
     * of a set of neighbouring pixels.
     * @param threshold the threshold used to determine whether an edge is present or not.
     */
    void setLumaThreshold(float threshold);

    /**
     * @return the reduce multiplier.
     */
    float getReduceMultiplier() const { return mReduceMultiplier; }
     
    /**
     * @return the min value for the return multiplier. 
     */
    float getMinReduceMultiplier() const { return mMinReduce; }

    /**
     * @return the max span used when blurring edges. 
     */
    float getMaxSpan() const { return mMaxSpan; }

    /**
     * @return the luma threshold.
     */
    float getLumaThreshold() const { return mLumaThreshold; }

    virtual void onSetup(Shader& postProcessingShader) override;
};
