#include "EffectsFBO.h"
#include <iostream>

void EffectsFBO::init(std::uint32_t width, std::uint32_t height)
{
	mColorBuffer = Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_RGB);
	mDepthBuffer = Texture::load(nullptr, width, height, GL_REPEAT, GL_REPEAT, false, GL_DEPTH_COMPONENT, GL_FLOAT);

	glGenFramebuffers(1, &mFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBuffer.getId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthBuffer.getId(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Render to texture frame buffer is incomplete\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::uint32_t EffectsFBO::getFbo() const
{
	return mFbo;
}

const Texture& EffectsFBO::getColorBuffer() const
{
	return mColorBuffer;
}

const Texture& EffectsFBO::getDepthBuffer() const
{
	return mDepthBuffer;
}

EffectsFBO::~EffectsFBO()
{
	glDeleteFramebuffers(1, &mFbo);
}
