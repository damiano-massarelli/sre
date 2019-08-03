#include "PBRMaterial.h"



PBRMaterial::PBRMaterial()
	: Material{ {"shaders/bumpedPhongVS.glsl"}, {"shaders/pbrFS.glsl"} }
{
	shader.use();

	shader.setInt("material.albedo", 0);
	shader.setInt("material.normal", 1);
	shader.setInt("material.roughness", 2);
	shader.setInt("material.metalness", 3);
	shader.setInt("material.ao", 4);

}


void PBRMaterial::setAlbedo(const Texture& albedo)
{
	mAlbedo = albedo;
}

void PBRMaterial::setNormalMap(const Texture& normal)
{
	mNormal = normal;
}

void PBRMaterial::setMetalnessMap(const Texture& metalness)
{
	mMetalness = metalness;
}

void PBRMaterial::setRoughnessMap(const Texture& roughness)
{
	mRoughness = roughness;
}

void PBRMaterial::setAmbienOccludionMap(const Texture& ao)
{
	mAO = ao;
}

void PBRMaterial::use()
{
	shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mAlbedo.getId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mNormal.getId());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mRoughness.getId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mMetalness.getId());

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mAO.getId());
}

void PBRMaterial::after()
{
	for (int i = 4; i <= 0; --i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

std::size_t PBRMaterial::hash() const
{
	return Material::hash()
		+ mAlbedo.getId()
		+ mNormal.getId()
		+ mMetalness.getId()
		+ mRoughness.getId()
		+ mAO.getId();
}

bool PBRMaterial::equalsTo(const Material* rhs) const
{
	// avoid dynamic cast. Same materials will have the same shader id
	// FIXME add type to Material
	if (shader.getId() != rhs->shader.getId()) return false;

	auto other = static_cast<const PBRMaterial*>(rhs);

	return Material::equalsTo(rhs)
		&& mAlbedo.getId() == other->mAlbedo.getId()
		&& mNormal.getId() == other->mNormal.getId()
		&& mMetalness.getId() == other->mMetalness.getId()
		&& mRoughness.getId() == other->mRoughness.getId()
		&& mAO.getId() == other->mAO.getId();
}

PBRMaterial::~PBRMaterial()
{
}
