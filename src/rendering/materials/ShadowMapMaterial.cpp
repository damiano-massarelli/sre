#include "ShadowMapMaterial.h"
#include "Engine.h" 

ShadowMapMaterial::ShadowMapMaterial() : Material{"shaders/shadowMapVS.glsl", "shaders/shadowMapFS.glsl"}
{
	shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void ShadowMapMaterial::use()
{
	shader.use();
}