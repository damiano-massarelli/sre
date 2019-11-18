#pragma once

#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"

/**
 * Applies multiple textures to a single object.
 * Up to 4 textures are supported and are blend together using
 * another texture which contains how much of each texture should be used
 * in the red, green and blue channels.
 */
class MultiTextureLambertMaterial : public Material
{
public:
	/** Base color texture (100% if blendTexture is (0, 0, 0)) */
	Texture baseTexture;

	/** color texture corresponding to red component of blendTexture (100% if blendTexture is (1, 0, 0)) */
	Texture redTexture;

	/** color texture corresponding to green component of blendTexture (100% if blendTexture is (0, 1, 0)) */
	Texture greenTexture;

	/** color texture corresponding to blue component of blendTexture (100% if blendTexture is (0, 0, 1)) */
	Texture blueTexture;

	/** blending texture */
	Texture blendTexture;

	/**
	 * Creates a new multi textured material.
	 * @param base base texture
	 * @param red texture corresponding to red channel in blend
	 * @param green texture corresponding to green channel in blend
	 * @param blue texture corresponding to blue channel in blend
	 * @param blend the blending texture
	 * @param horizontalTiles the number of texture tiles used by the textured mesh.
	 * @param verticalTiles the number of texture tiles used by the textured mesh (@see TerrainGenerator.setTextureTilesNumber).
	 */
	MultiTextureLambertMaterial(Texture base, Texture red, Texture green, Texture blue, Texture blend, float horizontalTiles = 1.0f, float verticalTiles = 1.0f);

	virtual void use() override;

	virtual void after() override;

	virtual std::size_t hash() const override;

	virtual bool equalsTo(const Material* rhs) const override;
};

