#pragma once

#include "RenderTarget.h"
#include "Texture.h"
#include "Shader.h"

/**
 * Performs a Gaussian blur on a Texture.
 * The GaussianBlur is carried out by reducing the 
 * original texture by a certain scale factor and
 * then performing a number of vertical and horizontal
 * blur stages (based on a Gaussian curve).
 */
class GaussianBlur
{
private:
	Texture mBlurred;

	RenderTarget mTarget;

	Shader hBlur;
	Shader vBlur;

public:
	/**
	 * Creates a GaussianBlur object.
	 * A lower value for scaleFactor will result in a more blurred Texture. However, a low
	 * scaleFactor will also introduce some flickering. Using a higher scaleFactor will reduce
	 * the flickering but the blur effect will be less visible. To circumvent this last issue it is
	 * possible to use more iterations of the blurring algorithm (see GaussianBlur::getBlurred)
	 * @param scaleFactor the scale factor used to scale the original Texture before blurring.
	 */
	GaussianBlur(float scaleFactor = 0.5);

	/**
	 * Blurs a Texture.
	 * @param src the Texture to be blurred.
	 * @param iterations how many times the blurring algorithm is executed.
	 * @return a blurred version of the src Texture.
	 */
	const Texture& getBlurred(const Texture& src, int iterations);

	~GaussianBlur();
};

