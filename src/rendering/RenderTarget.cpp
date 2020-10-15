#include "rendering/RenderTarget.h"
#include <glad/glad.h>
#include <iostream>

void RenderTarget::create(std::uint32_t width, std::uint32_t height, bool wantColorBuffer, bool wantDepthBuffer) {
    Texture colorBuffer;
    Texture depthBuffer;

    Texture::Settings colorLoadOptions;
    colorLoadOptions.internalFormat = GL_RGBA16F;
    colorLoadOptions.dataPixelType = GL_FLOAT;
    colorLoadOptions.appearanceOptions.wrapS = GL_REPEAT;
    colorLoadOptions.appearanceOptions.wrapT = GL_REPEAT;
    colorLoadOptions.appearanceOptions.createMipmap = false;

    Texture::Settings depthLoadOptions = colorLoadOptions;
    depthLoadOptions.internalFormat = GL_DEPTH24_STENCIL8;
    depthLoadOptions.dataPixelFormat = GL_DEPTH_STENCIL;
    depthLoadOptions.dataPixelType = GL_UNSIGNED_INT_24_8;

    if (wantColorBuffer)
        colorBuffer = Texture::load(nullptr, width, height, colorLoadOptions);

    if (wantDepthBuffer)
        depthBuffer = Texture::load(nullptr, width, height, depthLoadOptions);

    if (colorBuffer || depthBuffer)
        createWith(colorBuffer, depthBuffer);

    // update anyway
    mWidth = width;
    mHeight = height;
}

void RenderTarget::createWith(const Texture& colorBuffer, const Texture& depthBuffer) {
    if (colorBuffer) {
        mWidth = colorBuffer.getWidth();
        mHeight = colorBuffer.getHeight();
    } else if (depthBuffer) {
        mWidth = depthBuffer.getWidth();
        mHeight = depthBuffer.getHeight();
    }

    mColorBuffer = colorBuffer;
    mDepthBuffer = depthBuffer;

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    if (colorBuffer)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer.getId(), 0);
    else
        glDrawBuffer(GL_NONE);

    if (depthBuffer) {
        if (depthBuffer.isCubeMap())
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthBuffer.getId(), 0);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.getId(), 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "RenderTarget frame buffer is incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::uint32_t RenderTarget::getFbo() const { return mFbo; }

std::uint32_t RenderTarget::getWidth() const { return mWidth; }

std::uint32_t RenderTarget::getHeight() const { return mHeight; }

const Texture& RenderTarget::getColorBuffer() const { return mColorBuffer; }

const Texture& RenderTarget::getDepthBuffer() const { return mDepthBuffer; }

bool RenderTarget::isValid() const { return mColorBuffer.isValid() || mDepthBuffer.isValid(); }

RenderTarget& RenderTarget::operator=(const RenderTarget& rhs) {
    if (mFbo == rhs.mFbo)
        return *this;

    cleanUpIfNeeded();

    mFbo = rhs.mFbo;
    mColorBuffer = rhs.mColorBuffer;
    mDepthBuffer = rhs.mDepthBuffer;

    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;

    mRefCoutner = rhs.mRefCoutner;

    return *this;
}

void RenderTarget::cleanUpIfNeeded() {
    if (mRefCoutner.shouldCleanUp() && mFbo != 0)
        glDeleteFramebuffers(1, &mFbo);
}

RenderTarget::~RenderTarget() { cleanUpIfNeeded(); }
