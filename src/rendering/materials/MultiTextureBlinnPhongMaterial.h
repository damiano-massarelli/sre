#pragma once
#include "Material.h"
#include "Texture.h"
#include <cstdint>

/**
 * A Material used for terrains that need specular lights and bump mapping.
 * Textures are blended together based on the black, red and green values 
 * contained in the blendTexture. Unlike MultiTextureLamberMaterial, the blue
 * component is not used.
 * Since this Material support bump will work correctly only if the corresponding
 * mesh has tangent space information in it.
 */
class MultiTextureBlinnPhongMaterial :
	public Material
{
private:
	std::int32_t mRedShininessLocation = -1;
	std::int32_t mGreenShininessLocation = -1;

public:
	/** Base color texture (100% if blendTexture is (0, 0, 0)) */
	Texture baseTexture;
	/** Bump map base texture (100% if blendTexture is (0, 0, 0) */
	Texture baseTextureBump;

	/** Diffuse color texture corresponding to red component of blendTexture (100% if blendTexture is (1, 0, 0)) */
	Texture redTexture;
	/** Specular color texture corresponding to the red component of blendTexture */
	Texture redTextureSpecular;
	/** Bump map corresponding to the red component of blendTexture */
	Texture redTextureBump;
	/** Shininess corresponding to the red component of blendTexture */
	float redShininess = 32.0f; 

	/** diffuse color texture corresponding to green component of blendTexture (100% if blendTexture is (0, 1, 0)) */
	Texture greenTexture;
	/** Specular color texture corresponding to the green component of blendTexture */
	Texture greenTextureSpecular;
	/** Bump map corresponding to the green component of blendTexture */
	Texture greenTextureBump;
	/** Shininess corresponding to the green component of blendTexture */
	float greenShininess = 32.0f;

	/** blending texture */
	Texture blendTexture;

	MultiTextureBlinnPhongMaterial();

	virtual void use() override;

	virtual void after() override;

	~MultiTextureBlinnPhongMaterial() = default;
};

