#pragma once
#include "Texture.h"
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
	Texture mNonDeferredBuffer;

public:
	DeferredRenderingFBO() = default;

	void init(std::uint32_t width, std::uint32_t height);

	std::uint32_t getFBO() const;

	const Texture& getDiffuseBuffer() const;

	const Texture& getSpecularBuffer() const;

	const Texture& getPositionBuffer() const;

	const Texture& getNormalBuffer() const;

	const Texture& getDepthBuffer() const;

	const Texture& getNonDeferredBuffer() const;

	~DeferredRenderingFBO();
};

