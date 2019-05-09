#include "LightMaterial.h"
#include "Engine.h"

LightMaterial::LightMaterial() : Material{"shaders/lightVS.glsl", "shaders/lightFS.glsl"}
{
    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void LightMaterial::use()
{
    shader.use();
    shader.setVec3("color", color);
}
