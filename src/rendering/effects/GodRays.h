#pragma once
#include "Effect.h"
#include "GameObjectEH.h"
#include "RenderTarget.h"
#include "Shader.h"

/**
 * Good rays effect.
 */
class GodRays :
	public Effect
{
private:
	RenderTarget mBlurredTarget;

	int mBlurredTexture;

	Shader mBlurredSiluetteCreator;

public:
	/** The light emitting god rays */
	GameObjectEH light;

	GodRays();

	virtual void onSetup(Shader& postProcessingShader) override;

	virtual void update(Shader& postProcessingShader) override;

	virtual ~GodRays();
};

