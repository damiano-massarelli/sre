#pragma once
#include "Effect.h"
#include <glm/glm.hpp>
#include <cstdint>

class MotionBlur :
	public Effect
{
private:
	glm::mat4 mPrevProjViewMatrix;
	std::int32_t mPrevProjViewMatrixLocation = 0;
	std::int32_t mCurrentProjViewMatrixLocation = 0;

	int mPositionTexture = -1;

	float mBlurFactor = 100.0f;
	bool mBlurNeedsUpdate = false;

public:
	MotionBlur();

	void setBlurFactor(float blurFactor);

	virtual void onSetup(Shader& postProcessingShader) override;

	virtual void update(Shader& postProcessingShader) override;

	~MotionBlur();
};

