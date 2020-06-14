#pragma once
#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"

class PBRMaterial :
	public Material
{
private:
	Texture mAlbedo;
	Texture mNormal;
	Texture mMetalness;
	Texture mRoughness;
	Texture mAO;

public:
	PBRMaterial();

	void setAlbedo(const Texture& albedo);

	void setNormalMap(const Texture& normal);

	void setMetalnessMap(const Texture& metalness);

	void setRoughnessMap(const Texture& roughness);

	void setAmbientOcclusionMap(const Texture& ao);

	virtual void use() override;

	virtual void after() override;

	virtual std::size_t hash() const override;

	virtual bool equalsTo(const Material* rhs) const override;

	~PBRMaterial();
};

