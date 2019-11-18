#pragma once
#include "rendering/materials/Texture.h"
#include "resourceManagment/RefCount.h"
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

private:
	void cleanUpIfNeeded();

public:
	RenderTarget() = default;

	/**
	 * Creates a RenderTarget with a color and a depth buffer.
	 * @param width the width of the textures
	 * @param height the height of the textures
	 * @param wantColorBuffer whether or not to add a color buffer to this RenderTarget
	 * @param wantDepthBuffer whether or not to add a depth buffer to this RenderTarget
	 */
	void create(std::uint32_t width, std::uint32_t height, bool wantColorBuffer = true, bool wantDepthBuffer = true);

	/**
	 * Creates a RenderTarget with the given Texture%s.
	 * @param colorBuffer the Texture to use as color buffer. If this texture is invalid, this RenderTarget won't have a color buffer.
	 * @param depthBuffer the Texture to use as depth buffer. If this texture is invalid, this RenderTarget won't have a depth buffer.
	 */
	void createWith(const Texture& colorBuffer, const Texture& depthBuffer);

	/**
	 * @return the FBO id for this render target.
	 */
	std::uint32_t getFbo() const;

	/**
	 * @return the width of the color and depth buffers
	 */
	std::uint32_t getWidth() const;

	/**
	 * @return the height of the color and depth buffers
	 */
	std::uint32_t getHeight() const;

	/**
	 * @return the color buffer
	 */
	const Texture& getColorBuffer() const;

	/**
	 * @return the depth buffer
	 */
	const Texture& getDepthBuffer() const;

	/**
	 * @return true if one of the color or depth buffer is valid
	 */
	bool isValid() const;

	RenderTarget& operator=(const RenderTarget& rhs);

	~RenderTarget();
};

