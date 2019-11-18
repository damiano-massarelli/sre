#include "rendering/materials/MultiTextureBlinnPhongMaterial.h"
#include "rendering/RenderSystem.h"

MultiTextureBlinnPhongMaterial::MultiTextureBlinnPhongMaterial() :
	Material{"shaders/bumpedPhongVS.glsl",
		     "shaders/multiTexturePhongFS.glsl" }
{
	unSupportedRenderPhases |= RenderPhase::FORWARD_RENDERING;

	shader.use();
	
	shader.setInt("baseTexture", 0);
	shader.setInt("baseTextureBump", 1);
	shader.setInt("redTexture", 2);
	shader.setInt("redTextureSpecular", 3);
	shader.setInt("redTextureBump", 4);
	mRedShininessLocation = shader.getLocationOf("redShininess");
	shader.setInt("greenTexture", 5);
	shader.setInt("greenTextureSpecular", 6);
	shader.setInt("greenTextureBump", 7);
	shader.setInt("blendTexture", 8);
	mGreenShininessLocation = shader.getLocationOf("greenShininess");
}


void MultiTextureBlinnPhongMaterial::use()
{
	shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, baseTexture.getId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, baseTextureBump.getId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, redTexture.getId());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, redTextureSpecular.getId());

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, redTextureBump.getId());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, greenTexture.getId());

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, greenTextureSpecular.getId());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, greenTextureBump.getId());

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, blendTexture.getId());

	shader.setFloat(mRedShininessLocation, redShininess);
	shader.setFloat(mGreenShininessLocation, greenShininess);
}

void MultiTextureBlinnPhongMaterial::after()
{
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::size_t MultiTextureBlinnPhongMaterial::hash() const
{
	return Material::hash()
		+ baseTexture.getId()
		+ baseTextureBump.getId()
		+ redTexture.getId()
		+ redTextureSpecular.getId()
		+ redTextureBump.getId()
		+ greenTexture.getId()
		+ greenTextureSpecular.getId()
		+ greenTextureBump.getId()
		+ blendTexture.getId();
}

bool MultiTextureBlinnPhongMaterial::equalsTo(const Material* rhs) const
{
	if (shader.getId() != rhs->shader.getId())
		return false;

	auto other = static_cast<const MultiTextureBlinnPhongMaterial*>(rhs);
	return Material::equalsTo(rhs)
		&& baseTexture.getId() == other->baseTexture.getId()
		&& baseTextureBump.getId() == other->baseTextureBump.getId()
		&& redTexture.getId() == other->redTexture.getId()
		&& redTextureSpecular.getId() == other->redTextureSpecular.getId()
		&& redTextureBump.getId() == other->redTextureBump.getId()
		&& greenTexture.getId() == other->greenTexture.getId()
		&& greenTextureSpecular.getId() == other->greenTextureSpecular.getId()
		&& greenTextureBump.getId() == other->greenTextureBump.getId()
		&& blendTexture.getId() == other->blendTexture.getId()
		&& redShininess == other->redShininess
		&& greenShininess == other->greenShininess;
}
