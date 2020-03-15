#pragma once
#include "Effect.h"
#include "rendering/materials/Shader.h"

class SSR : public Effect
{
private:
	int mPositionTexture = -1;
	int mNormalTexture = -1;
	int mSpecularTexture = -1;

public:
	SSR();

	virtual void onSetup(Shader& postProcessingShader) override;

	virtual void update(Shader& postProcessingShader) override;
};

