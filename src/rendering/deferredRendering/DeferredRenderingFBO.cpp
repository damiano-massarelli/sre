#include "rendering/deferredRendering/DeferredRenderingFBO.h"
#include <iostream>
#include <utility>

GBuffer::GBuffer(std::uint32_t width, std::uint32_t height) {
    mWidth = width;
    mHeight = height;

    mDiffuseBuffer = Texture::load(nullptr, width, height, DIFFUSE_BUFFER_SETTINGS);
    mMaterialBuffer = Texture::load(nullptr, width, height, MATERIAL_BUFFER_SETTINGS);
    mPositionBuffer = Texture::load(nullptr, width, height, POSITION_BUFFER_SETTINGS);
    mNormalBuffer = Texture::load(nullptr, width, height, NORMAL_BUFFER_SETTINGS);
    mDepthBuffer = Texture::load(nullptr, width, height, DEPTH_BUFFER_SETTINGS);

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDiffuseBuffer.getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mMaterialBuffer.getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mPositionBuffer.getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mNormalBuffer.getId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer.getId(), 0);

    unsigned int attachments[]{
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(4, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Deferred rendering buffer is incomplete\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::uint32_t GBuffer::getFBO() const {
    return mFbo;
}

const Texture& GBuffer::getDiffuseBuffer() const {
    return mDiffuseBuffer;
}

const Texture& GBuffer::getMaterialBuffer() const {
    return mMaterialBuffer;
}

const Texture& GBuffer::getPositionBuffer() const {
    return mPositionBuffer;
}

const Texture& GBuffer::getNormalBuffer() const {
    return mNormalBuffer;
}

const Texture& GBuffer::getDepthBuffer() const {
    return mDepthBuffer;
}

std::uint32_t GBuffer::getWidth() const {
    return mWidth;
}

std::uint32_t GBuffer::getHeight() const {
    return mHeight;
}

GBuffer::GBuffer(GBuffer&& rhs) noexcept {
    *this = std::move(rhs);
}

GBuffer& GBuffer::operator=(GBuffer&& rhs) noexcept {
    cleanUp();
    mFbo = rhs.mFbo;
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mDiffuseBuffer = std::move(rhs.mDiffuseBuffer);
    mPositionBuffer = std::move(rhs.mPositionBuffer);
    mNormalBuffer = std::move(rhs.mNormalBuffer);
    mMaterialBuffer = std::move(rhs.mMaterialBuffer);
    mDepthBuffer = std::move(rhs.mDepthBuffer);

    rhs.mFbo = 0;

#ifdef SRE_DEBUG
    // Helpful hints to detect the use of a moved GBuffer
    rhs.mWidth = 0;
    rhs.mHeight = 0;
    rhs.mDiffuseBuffer = Texture{};
    rhs.mNormalBuffer = Texture{};
    rhs.mMaterialBuffer = Texture{};
    rhs.mDepthBuffer = Texture{};
#endif  // SRE_DEBUG

    return *this;
}

void GBuffer::cleanUp() {
    if (mFbo != 0) {
        glDeleteFramebuffers(1, &mFbo);
    }
}

GBuffer::~GBuffer() {
    cleanUp();
}
