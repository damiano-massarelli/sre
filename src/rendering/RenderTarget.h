#pragma once
#include "rendering/materials/Texture.h"
#include "resourceManagment/RefCount.h"
#include <cstdint>

/**
 * A RenderTarget allows rendering scenes to a texture.
 * A RenderTarget can be created specifying one of two textures independently:
 * a color buffer and a depth buffer.
 * RenderTarge%s do not retain the textures used to create it so it is your
 * responsibility to make sure that the RenderTarget does not outlive the texture it was
 * created with.
 * Creating a RenderTarget can be expensive. It is often convenient to create a Texture and
 * the corresponding render target only once.
 */
class RenderTarget {
private:
    std::uint32_t mFbo = 0;

    const Texture* mColorBuffer = nullptr;
    const Texture* mDepthBuffer = nullptr;

    std::uint32_t mWidth = 0;
    std::uint32_t mHeight = 0;

    void cleanUp();

public:
    /**
     * Creates an invalid RenderTarget.
     */
    RenderTarget() = default;

    /**
     * Creates a new RenderTarget.
     * It is possible to provide a colorBuffer and/or a depthBuffer or both.
     * However, it is an error to create a RenderTarget without any buffer.
     * The given texture are not retained. Make sure that this RenderTarget does not
     * outlive the textures it was created with.
     */
    explicit RenderTarget(const Texture* colorBuffer, const Texture* depthBuffer);

    RenderTarget(RenderTarget&& rhs) noexcept;

    RenderTarget(const RenderTarget&) = delete;

    RenderTarget& operator=(const RenderTarget&) = delete;

    RenderTarget& operator=(RenderTarget&& rhs) noexcept;

    ~RenderTarget();

    /**
     * @return the FBO id for this render target.
     */
    std::uint32_t getFbo() const;

    /**
     * @return the color buffer.
     */
    const Texture* getColorBuffer() const { return mColorBuffer; }

    /**
     * @return the depth buffer.
     */
    const Texture* getDepthBuffer() const { return mDepthBuffer; }

    /**
     * @return the width of the color and depth buffers
     */
    std::uint32_t getWidth() const;

    /**
     * @return the height of the color and depth buffers
     */
    std::uint32_t getHeight() const;

    /**
     * @return true if one of the color or depth buffer is valid
     */
    [[nodiscard]] bool isValid() const;
};
