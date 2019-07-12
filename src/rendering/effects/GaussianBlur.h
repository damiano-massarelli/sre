#pragma once

#include "RenderTarget.h"
#include "Texture.h"
#include "Shader.h"

class GaussianBlur
{
private:
	Texture mBlurred;

	RenderTarget mTarget;

	Shader hBlur;
	Shader vBlur;

public:
	GaussianBlur(float scaleFactor = 0.5);

	const Texture& getBlurred(const Texture& src, int iterations);

	~GaussianBlur();
};

