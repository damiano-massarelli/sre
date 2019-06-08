#pragma once

#include "Effect.h"

class FXAA : public Effect
{
public:
	FXAA();

	virtual void onSetup(Shader& postProcessingShader) override;

	~FXAA();
};

