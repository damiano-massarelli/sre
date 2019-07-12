#pragma once
#include "Effect.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "GaussianBlur.h"

class Bloom :
	public Effect
{
private:
	Shader mBloomExtractor;
	RenderTarget mTarget;
	Texture mBloom;
	GaussianBlur mGaussianBlur;

public:
	Bloom();

	virtual void onSetup(Shader& postProcessingShader) override;

	virtual void update(Shader& postProcessingShader) override;

	~Bloom();
};

