#include "RenderTarget.h"
#include <glad/glad.h>
#include <iostream>

void RenderTarget::create(std::uint32_t width, std::uint32_t height, bool wantColorBuffer, bool wantDepthBuffer)
{
	Texture colorBuffer;
	Texture depthBuffer;

	if (wantColorBuffer)
		colorBuffer = Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_RGBA);

	if (wantDepthBuffer)
		depthBuffer = Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH24_STENCIL8);


	createWith(colorBuffer, depthBuffer);
}

void RenderTarget::createWith(const Texture& colorBuffer, const Texture& depthBuffer)
{
	mWidth = colorBuffer.getWidth();
	mHeight = colorBuffer.getHeight();

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
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR)
			std::cout << error << "\n";
		std::cout << "RenderTarget frame buffer is incomplete\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::uint32_t RenderTarget::getFbo() const
{
	return mFbo;
}

std::uint32_t RenderTarget::getWidth() const
{
	return mWidth;
}

std::uint32_t RenderTarget::getHeight() const
{
	return mHeight;
}

const Texture& RenderTarget::getColorBuffer() const
{
	return mColorBuffer;
}

const Texture& RenderTarget::getDepthBuffer() const
{
	return mDepthBuffer;
}

RenderTarget::~RenderTarget()
{
	if (mRefCoutner.shouldCleanUp() && mFbo != 0)
		glDeleteFramebuffers(1, &mFbo);
}
