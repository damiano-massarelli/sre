#pragma once

#include "rendering/RenderTarget.h"
#include "rendering/deferredRendering/DeferredRenderingFBO.h"
#include "rendering/materials/Shader.h"
#include "rendering/materials/Texture.h"
#include <memory>

/**
 * Performs a Gaussian blur on a Texture.
 * The GaussianBlur is carried out by reducing the
 * original texture by a certain scale factor and
 * then performing a number of vertical and horizontal
 * blur stages (based on a Gaussian curve).
 */
class GaussianBlur {
private:
    std::unique_ptr<Texture> mHorizontalBlurred;
    RenderTarget mHorizontalTarget;

    // the following texture and render target are
    // valid only if the GaussianBlur object is created without passing
    // a render target.
    std::unique_ptr<Texture> mCombinedBlurred;
    RenderTarget mCombinedTarget;

    RenderTarget* mResultBuffer = nullptr;

    Shader mHBlur;
    Shader mVBlur;
    Shader mPassThrough;

    void initRenderTargets(RenderTarget* resultBuffer);

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
     * @param outputSettings used to specify the Texture format and general settings
     * for the result. Some settings are imposed, e.g. mpimaps are always disabled.
     */
    GaussianBlur(float scaleFactor = 0.5F, Texture::Settings outputSettings = GBuffer::DIFFUSE_BUFFER_SETTINGS);

    /**
     * Creates a GaussianBlur object.
     * The result of the applied blur effect will be written directly into the given
     * RenderTarget. The smaller the resolution of the given RenderTarget the more the
     * final image will be blurred. However this will also introduce some flickering.
     * To circumvent this last issue it is possible to use more
     * iterations of the blurring algorithm (see GaussianBlur::getBlurred)
     * @param renderTarget the RenderTarget used to write the result of the blur
     * effect. The Texture returned by getBlurred is the same as renderTarget->getColorBuffer().
     */
    GaussianBlur(RenderTarget* renderTarget);

    /**
     * Blurs a Texture.
     * @param src the Texture to be blurred.
     * @param iterations how many times the blurring algorithm is executed.
     * @return a blurred version of the src Texture.
     */
    const Texture& getBlurred(const Texture& src, int iterations);
};
