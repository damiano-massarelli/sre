#pragma once

#include "Material.h"
#include "Texture.h"

/**
 * Applies multiple textures to a single object.
 * Up to 4 textures are supported and are blend together using
 * another texture which contains in the red, green and blue channels
 * how much of each texture should be used.
 */
class MultiTextureLambertMaterial : public Material
{
public:
	Texture baseTexture;
	Texture redTexture;
	Texture greenTexture;
	Texture blueTexture;
	Texture blendTexture;

	MultiTextureLambertMaterial(Texture base, Texture red, Texture green, Texture blue, Texture blend);

	virtual void use() override;

	virtual void after() override;

};

