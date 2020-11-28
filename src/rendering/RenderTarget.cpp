#include "rendering/RenderTarget.h"
#include <cassert>
#include <glad/glad.h>
#include <utility>

RenderTarget::RenderTarget(const Texture* colorBuffer, const Texture* depthBuffer, std::int32_t colorLevel, std::int32_t depthLevel)
    : mColorBuffer{ colorBuffer }
    , mDepthBuffer{ depthBuffer }
    , mColorLevel{ colorLevel }
    , mDepthLevel{ depthLevel } {

    // Get the currently bound texture
    GLint currentlyBoundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentlyBoundTexture);

    GLint width = 0;
    GLint height = 0;
    if (colorBuffer != nullptr && colorBuffer->isValid()) {
        // retrieve width and height for the given level
        glBindTexture(GL_TEXTURE_2D, colorBuffer->getId());
        glGetTexLevelParameteriv(GL_TEXTURE_2D, static_cast<GLint>(colorLevel), GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, static_cast<GLint>(colorLevel), GL_TEXTURE_HEIGHT, &height);

    } else if (depthBuffer != nullptr && depthBuffer->isValid()) {
        // retrieve width and height for the given level
        glBindTexture(GL_TEXTURE_2D, depthBuffer->getId());
        glGetTexLevelParameteriv(GL_TEXTURE_2D, static_cast<GLint>(colorLevel), GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, static_cast<GLint>(colorLevel), GL_TEXTURE_HEIGHT, &height);
    }

    // restore previous texture
    glBindTexture(GL_TEXTURE_2D, currentlyBoundTexture);

    mWidth = static_cast<std::uint32_t>(width);
    mHeight = static_cast<std::uint32_t>(height);

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    if (colorBuffer != nullptr && colorBuffer->isValid()) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->getId(), static_cast<GLint>(colorLevel));
    } else {
        glDrawBuffer(GL_NONE);
    }

    if (depthBuffer != nullptr && depthBuffer->isValid()) {
        if (depthBuffer->isCubeMap()) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthBuffer->getId(), depthLevel);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer->getId(), depthLevel);
        }
    }

#ifdef SRE_DEBUG
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#endif  // SRE_DEBUG

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::RenderTarget(const Texture* colorBuffer, const Texture* depthBuffer) 
    : RenderTarget{colorBuffer, depthBuffer, 0, 0}
{
}

RenderTarget::RenderTarget(RenderTarget&& rhs) noexcept { *this = std::move(rhs); }

RenderTarget& RenderTarget::operator=(RenderTarget&& rhs) noexcept {
    cleanUp();
    mFbo = rhs.mFbo;
    rhs.mFbo = 0;
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mColorBuffer = rhs.mColorBuffer;
    mDepthBuffer = rhs.mDepthBuffer;
    mColorLevel = rhs.mColorLevel;
    mDepthLevel = rhs.mDepthLevel;

#ifdef SRE_DEBUG
    // Helpful hints to detect the use of a moved RenderTarget
    rhs.mWidth = 0;
    rhs.mHeight = 0;
    rhs.mColorBuffer = nullptr;
    rhs.mDepthBuffer = nullptr;
    rhs.mColorLevel = 0;
    rhs.mDepthLevel = 0;
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

std::int32_t RenderTarget::getColorBufferMipMapLevel() const
{
    return mColorLevel;
}

std::int32_t RenderTarget::getDepthBufferMipMapLevel() const
{
    return mDepthLevel;
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
