#pragma once

#include "rendering/RenderTarget.h"
#include "rendering/materials/Shader.h"
#include "rendering/materials/Texture.h"

/**
 * Performs a Gaussian blur on a Texture.
 * The GaussianBlur is carried out by reducing the
 * original texture by a certain scale factor and
 * then performing a number of vertical and horizontal
 * blur stages (based on a Gaussian curve).
 */
class GaussianBlur {
private:
    Texture mHorizontalBlurred;
    RenderTarget mHorizontalTarget;

    Texture mCombinedBlurred;
    RenderTarget mCombinedTarget;

    Shader hBlur;
    Shader vBlur;

public:
    /**
     * Creates a GaussianBlur object.
     * A lower value for scaleFactor will result in a more blurred Texture.
     * However, a low scaleFactor will also introduce some flickering. Using a
     * higher scaleFactor will reduce the flickering but the blur effect will be
     * less visible. To circumvent this last issue it is possible to use more
     * iterations of the blurring algorithm (see GaussianBlur::getBlurred)
     * @param scaleFactor the scale factor used to scale the original Texture
     * before blurring.
     */
    GaussianBlur(float scaleFactor = 0.5F);

    /**
     * Blurs a Texture.
     * @param src the Texture to be blurred.
     * @param iterations how many times the blurring algorithm is executed.
     * @return a blurred version of the src Texture.
     */
    const Texture& getBlurred(const Texture& src, int iterations);

    ~GaussianBlur();
};
