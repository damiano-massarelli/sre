#include "rendering/RenderTarget.h"
#include <cassert>
#include <glad/glad.h>
#include <utility>

RenderTarget::RenderTarget(const Texture* colorBuffer, const Texture* depthBuffer)
    : mColorBuffer{ colorBuffer }
    , mDepthBuffer{ depthBuffer } {
    if (colorBuffer != nullptr && colorBuffer->isValid()) {
        mWidth = colorBuffer->getWidth();
        mHeight = colorBuffer->getHeight();
    } else if (depthBuffer != nullptr && depthBuffer->isValid()) {
        mWidth = depthBuffer->getWidth();
        mHeight = depthBuffer->getHeight();
    }

    mColorBuffer = colorBuffer;
    mDepthBuffer = depthBuffer;

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    if (colorBuffer != nullptr && colorBuffer->isValid()) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->getId(), 0);
    } else {
        glDrawBuffer(GL_NONE);
    }

    if (depthBuffer != nullptr && depthBuffer->isValid()) {
        if (depthBuffer->isCubeMap()) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthBuffer->getId(), 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->getId(), 0);
        }
    }

#ifdef SRE_DEBUG
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#endif  // SRE_DEBUG

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::RenderTarget(RenderTarget&& rhs) noexcept {
    *this = std::move(rhs);
}

RenderTarget& RenderTarget::operator=(RenderTarget&& rhs) noexcept {
    cleanUp();
    mFbo = rhs.mFbo;
    rhs.mFbo = 0;
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mColorBuffer = rhs.mColorBuffer;
    mDepthBuffer = rhs.mDepthBuffer;

#ifdef SRE_DEBUG
    // Helpful hints to detect the use of a moved RenderTarget
    rhs.mWidth = 0;
    rhs.mHeight = 0;
    rhs.mColorBuffer = nullptr;
    rhs.mDepthBuffer = nullptr;
#endif  // SRE_DEBUG

    return *this;
}

std::uint32_t RenderTarget::getFbo() const {
    return mFbo;
}

std::uint32_t RenderTarget::getWidth() const {
    return mWidth;
}

std::uint32_t RenderTarget::getHeight() const {
    return mHeight;
}

bool RenderTarget::isValid() const {
    return mFbo != 0
        && ((mColorBuffer != nullptr && mColorBuffer->isValid())
            || (mDepthBuffer != nullptr && mDepthBuffer->isValid()));
}

void RenderTarget::cleanUp() {
    if (mFbo != 0) {
        glDeleteFramebuffers(1, &mFbo);
        mFbo = 0;
    }
}

RenderTarget::~RenderTarget() {
    cleanUp();
}
