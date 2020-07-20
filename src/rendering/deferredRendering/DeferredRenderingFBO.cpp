#include "rendering/deferredRendering/DeferredRenderingFBO.h"
#include <iostream>

void DeferredRenderingFBO::init(std::uint32_t width, std::uint32_t height)
{
	mWidth = width;
	mHeight = height;

	Texture::TextureLoadOptions loadOptions;
	loadOptions.internalFormat = GL_RGBA16F;
	loadOptions.dataPixelType = GL_FLOAT;
	loadOptions.appearanceOptions.createMipmap = false;
	loadOptions.appearanceOptions.wrapS = GL_REPEAT;
	loadOptions.appearanceOptions.wrapT = GL_REPEAT;

	Texture::TextureLoadOptions depthLoadOptions = loadOptions;
	depthLoadOptions.internalFormat = GL_DEPTH24_STENCIL8;
	depthLoadOptions.dataPixelFormat = GL_DEPTH_STENCIL;
	depthLoadOptions.dataPixelType = GL_UNSIGNED_INT_24_8;

	mDiffuseBuffer		= Texture::load(nullptr, width, height, loadOptions);
	mAdditionalBuffer	= Texture::load(nullptr, width, height, loadOptions);
	mPositionBuffer		= Texture::load(nullptr, width, height, loadOptions);
	mNormalBuffer		= Texture::load(nullptr, width, height, loadOptions);
	mDepthBuffer		= Texture::load(nullptr, width, height, depthLoadOptions);

	glGenFramebuffers(1, &mFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDiffuseBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mAdditionalBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mPositionBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mNormalBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer.getId(), 0);

	unsigned int attachments[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Deferred rendering buffer is incomplete\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::uint32_t DeferredRenderingFBO::getFBO() const
{
	return mFbo;
}

void DeferredRenderingFBO::resize(std::uint32_t width, std::uint32_t height)
{
	if (width != mWidth || height != mHeight) {
		glDeleteFramebuffers(1, &mFbo);
		init(width, height);
	}
}

const Texture& DeferredRenderingFBO::getDiffuseBuffer() const
{
	return mDiffuseBuffer;
}

const Texture& DeferredRenderingFBO::getAdditionalBuffer() const
{
	return mAdditionalBuffer;
}

const Texture& DeferredRenderingFBO::getPositionBuffer() const
{
	return mPositionBuffer;
}

const Texture& DeferredRenderingFBO::getNormalBuffer() const
{
	return mNormalBuffer;
}

const Texture& DeferredRenderingFBO::getDepthBuffer() const
{
	return mDepthBuffer;
}

std::uint32_t DeferredRenderingFBO::getWidth() const
{
	return mWidth;
}

std::uint32_t DeferredRenderingFBO::getHeight() const
{
	return mHeight;
}

DeferredRenderingFBO& DeferredRenderingFBO::operator=(const DeferredRenderingFBO& rhs)
{
	if (mFbo == rhs.mFbo) return *this;

	cleanUpIfNeeded();

	mFbo = rhs.mFbo;
	mDiffuseBuffer = rhs.mDiffuseBuffer;
	mAdditionalBuffer = rhs.mAdditionalBuffer;
	mPositionBuffer = rhs.mPositionBuffer;
	mNormalBuffer = rhs.mNormalBuffer;
	mDepthBuffer = rhs.mDepthBuffer;

	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;

	mRefCount = rhs.mRefCount;

	return *this;
}

void DeferredRenderingFBO::cleanUpIfNeeded()
{
	if (mRefCount.shouldCleanUp() && mFbo != 0)
		glDeleteFramebuffers(1, &mFbo);
}

DeferredRenderingFBO::~DeferredRenderingFBO()
{
	cleanUpIfNeeded();
}

