#include "rendering/materials/MultiTextureLambertMaterial.h"
#include "rendering/RenderSystem.h"

MultiTextureLambertMaterial::MultiTextureLambertMaterial(Texture base, Texture red, Texture green, Texture blue, Texture blend, float horizontalTiles, float verticalTiles)
	: Material{ "shaders/phongVS.glsl",
				"shaders/multiTextureLambertFS.glsl" },
	baseTexture { base }, redTexture{ red }, greenTexture{ green }, blueTexture{ blue }, blendTexture{ blend }
{
	unSupportedRenderPhases |= RenderPhase::FORWARD_RENDERING;

	ShaderScopedUsage useShader{ shader };

	shader.setFloat("horizontalTiles", horizontalTiles);
	shader.setFloat("verticalTiles", verticalTiles);

	shader.setInt("baseTexture", 0);
	shader.setInt("redTexture", 1);
	shader.setInt("greenTexture", 2);
	shader.setInt("blueTexture", 3);
	shader.setInt("blendTexture", 4);
}

void MultiTextureLambertMaterial::use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, baseTexture.getId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, redTexture.getId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, greenTexture.getId());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, blueTexture.getId());

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, blendTexture.getId());

	shader.use();
}

void MultiTextureLambertMaterial::after()
{
	return;
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

	shader.stop();
}

std::size_t MultiTextureLambertMaterial::hash() const
{
	return Material::hash()
		+ baseTexture.getId()
		+ redTexture.getId()
		+ greenTexture.getId()
		+ blueTexture.getId();
}

bool MultiTextureLambertMaterial::equalsTo(const Material* rhs) const
{
	if (shader.getId() != rhs->shader.getId())
		return false;

	auto other = static_cast<const MultiTextureLambertMaterial*>(rhs);
	
	return Material::equalsTo(rhs)
		&& baseTexture.getId() == other->baseTexture.getId()
		&& redTexture.getId() == other->redTexture.getId()
		&& greenTexture.getId() == other->greenTexture.getId()
		&& blueTexture.getId() == other->blueTexture.getId();
}
