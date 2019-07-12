#pragma once
#include "Texture.h"
#include "RefCount.h"
#include <cstdint>
/**
 * A RenderTarget is basically a wrapper for an FBO.
 * RenderTargets can be passed to the RenderSystem in
 * order to render the scene to a texture.
 */
class RenderTarget
{
private:
	std::uint32_t mFbo = 0;
	Texture mColorBuffer;
	Texture mDepthBuffer;

	std::uint32_t mWidth = 0;
	std::uint32_t mHeight = 0;

	RefCount mRefCoutner;

public:
	RenderTarget() = default;

	void create(std::uint32_t width, std::uint32_t height);

	void createWith(const Texture& colorBuffer, const Texture& depthBuffer);

	std::uint32_t getFbo() const;

	std::uint32_t getWidth() const;

	std::uint32_t getHeight() const;

	const Texture& getColorBuffer() const;

	const Texture& getDepthBuffer() const;

	~RenderTarget();
};

