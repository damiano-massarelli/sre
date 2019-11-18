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

	virtual void update(Shader& postProcessingShader) override;
};

