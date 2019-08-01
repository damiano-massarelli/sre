#pragma once
#include "Texture.h"
#include "RefCount.h"
#include <cstdint>
/**
 * Manages the deferred rendering FBO and the Texture%s used
 * as render targets.
 */
class DeferredRenderingFBO
{
private:
	std::uint32_t mFbo;
	Texture mDiffuseBuffer;
	Texture mSpecularBuffer;
	Texture mPositionBuffer;
	Texture mNormalBuffer;
	Texture mDepthBuffer;

	std::uint32_t mWidth = 0;
	std::uint32_t mHeight = 0;

	RefCount mRefCount;

public:
	DeferredRenderingFBO() = default;

	void init(std::uint32_t width, std::uint32_t height);

	std::uint32_t getFBO() const;

	void resize(std::uint32_t width, std::uint32_t height);

	const Texture& getDiffuseBuffer() const;

	const Texture& getSpecularBuffer() const;

	const Texture& getPositionBuffer() const;

	const Texture& getNormalBuffer() const;

	const Texture& getDepthBuffer() const;

	std::uint32_t getWidth() const;

	std::uint32_t getHeight() const;

	~DeferredRenderingFBO();
};

