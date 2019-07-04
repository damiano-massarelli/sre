#include "Fog.h"
#include "Engine.h"

void Fog::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.bindUniformBlock("FogParams", RenderSystem::FOG_UNIFORM_BLOCK_INDEX);
	postProcessingShader.setInt("PositionData", 2);
}

void Fog::update(Shader& postProcessingShader)
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture();
}
