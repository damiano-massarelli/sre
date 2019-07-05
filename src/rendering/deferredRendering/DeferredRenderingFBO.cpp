#include "DeferredRenderingFBO.h"
#include <iostream>

void DeferredRenderingFBO::init(std::uint32_t width, std::uint32_t height)
{
	mWidth = width;
	mHeight = height;

	mDiffuseBuffer		= Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_RGBA);
	mSpecularBuffer		= Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_RGBA, GL_FLOAT, GL_RGBA16F);
	mPositionBuffer		= Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_RGB, GL_FLOAT, GL_RGB16F);
	mNormalBuffer		= Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_RGB, GL_FLOAT, GL_RGB16F);
	mDepthBuffer		= Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH24_STENCIL8);

	glGenFramebuffers(1, &mFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDiffuseBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mSpecularBuffer.getId(), 0);
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
	glDeleteFramebuffers(1, &mFbo);
	init(width, height);
}

const Texture& DeferredRenderingFBO::getDiffuseBuffer() const
{
	return mDiffuseBuffer;
}

const Texture& DeferredRenderingFBO::getSpecularBuffer() const
{
	return mSpecularBuffer;
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

DeferredRenderingFBO::~DeferredRenderingFBO()
{
	glDeleteFramebuffers(1, &mFbo);
}

