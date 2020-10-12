#pragma once
#include "rendering/materials/Texture.h"
#include "resourceManagment/RefCount.h"
#include <cstdint>

/**
 * Manages the deferred rendering FBO and the Texture%s used
 * as render targets.
 */
class DeferredRenderingFBO {
private:
    GLuint mFbo{};
    Texture mDiffuseBuffer;
    Texture mAdditionalBuffer;
    Texture mPositionBuffer;
    Texture mNormalBuffer;
    Texture mDepthBuffer;

    std::uint32_t mWidth = 0;
    std::uint32_t mHeight = 0;

    RefCount mRefCount;

    void cleanUpIfNeeded();

public:
    DeferredRenderingFBO() = default;

    /**
     * Creates the FBO and its textures with the given width and height.
     * @param width the width of the FBO
     * @param height the height of the FBO
     */
    void init(std::uint32_t width, std::uint32_t height);

    /**
     * Gets the FBO id
     * @return the numeric id for this FBO
     */
    GLuint getFBO() const;

    /**
     * Resizes the FBO.
     * @param width the width of the FBO
     * @param height the height of the FBO
     */
    void resize(std::uint32_t width, std::uint32_t height);

    /**
     * @return the buffer containing the diffuse color
     */
    const Texture& getDiffuseBuffer() const;

    /**
     * Returns an additional buffer.
     * This buffer is used for specular colors in the Blinn-Phong model
     * and for pbr data when PBR materials are used.
     * @return the buffer containing additional rendering information
     */
    const Texture& getAdditionalBuffer() const;

    /**
     * @return the buffer containing world space positions
     */
    const Texture& getPositionBuffer() const;

    /**
     * @return the buffer containing world space normals
     */
    const Texture& getNormalBuffer() const;

    /**
     * @return the depth buffer
     */
    const Texture& getDepthBuffer() const;

    /**
     * @return the width of this FBO
     */
    std::uint32_t getWidth() const;

    /*
     * @return the height of this FBO
     */
    std::uint32_t getHeight() const;

    DeferredRenderingFBO& operator=(const DeferredRenderingFBO& rhs);

    ~DeferredRenderingFBO();
};
