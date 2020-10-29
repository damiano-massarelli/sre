#pragma once
#include "rendering/materials/Texture.h"
#include <cstdint>

/**
 * A GBuffer is a framebuffer object composed of different Texture%s employed during deferred rendering.
 * The buffers held by this GBuffer can be retrieved using getDiffuseBuffer, getPositionBuffer, getNormalBuffer,
 * getMaterialBuffer and getDepthBuffer.
 */
class GBuffer {
private:
    GLuint mFbo{ 0 };
    Texture mDiffuseBuffer;
    Texture mMaterialBuffer;
    Texture mPositionBuffer;
    Texture mNormalBuffer;
    Texture mDepthBuffer;

    std::uint32_t mWidth = 0;
    std::uint32_t mHeight = 0;

    void cleanUp();

public:
    /**
     * Texture settings used for the diffuse buffer.
     */
    static constexpr Texture::Settings DIFFUSE_BUFFER_SETTINGS{
        GL_RGB16F, GL_RGB, GL_FLOAT, { false, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE }
    };

    /**
     * Texture settings used for the position buffer.
     */
    static constexpr Texture::Settings POSITION_BUFFER_SETTINGS = GBuffer::DIFFUSE_BUFFER_SETTINGS;

    /**
     * Texture settings used for the normal buffer.
     */
    static constexpr Texture::Settings NORMAL_BUFFER_SETTINGS = GBuffer::DIFFUSE_BUFFER_SETTINGS;

    /**
     * Texture settings used to store material-specific properties (roughness, metalness, ao)
     */
    static constexpr Texture::Settings MATERIAL_BUFFER_SETTINGS{
        GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, { false, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE }
    };

    /**
     * Texture settings used for the depth buffer.
     */
    static constexpr Texture::Settings DEPTH_BUFFER_SETTINGS{ GL_DEPTH24_STENCIL8,
        GL_DEPTH_STENCIL,
        GL_UNSIGNED_INT_24_8,
        { false, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE } };

    /**
     * Creates an invalid GBuffer
     */
    GBuffer() = default;

    /**
     * Creates the GBuffer and its textures with the given width and height.
     * @param width the width of the FBO
     * @param height the height of the FBO
     */
    GBuffer(std::uint32_t width, std::uint32_t height);

    GBuffer(const GBuffer& rhs) = delete;

    GBuffer(GBuffer&& rhs) noexcept;

    GBuffer& operator=(const GBuffer& rhs) = delete;

    GBuffer& operator=(GBuffer&& rhs) noexcept;

    /**
     * Gets the FBO id
     * @return the numeric id for this FBO
     */
    GLuint getFBO() const;

    /**
     * @return the buffer containing the diffuse color
     */
    const Texture& getDiffuseBuffer() const;

    /**
     * Returns the buffer containing PBR material properties.
     * The red channel contains the material's roughness.
     * The green channel contains the material's metalness.
     * The blue channel contains the material's ao.
     * @return the buffer containing PBR material properties.
     */
    const Texture& getMaterialBuffer() const;

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
     * @return the width of this GBuffer
     */
    std::uint32_t getWidth() const;

    /*
     * @return the height of this GBuffer
     */
    std::uint32_t getHeight() const;

    ~GBuffer();
};
