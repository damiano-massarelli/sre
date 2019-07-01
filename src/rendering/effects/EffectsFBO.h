#pragma once
#include "Texture.h"
#include <cstdint>

/**
 * Manages the FBO used to render effects.
 */
class EffectsFBO
{
private:
	std::uint32_t mFbo;
	Texture mColorBuffer;
	Texture mDepthBuffer;

public:
	EffectsFBO() = default;

	EffectsFBO(const EffectsFBO& efbo) = delete;

	EffectsFBO& operator=(const EffectsFBO& efbo) = delete;

	void init(std::uint32_t width, std::uint32_t height);

	std::uint32_t getFbo() const;

	const Texture& getColorBuffer() const;

	const Texture& getDepthBuffer() const;

	~EffectsFBO();
};

