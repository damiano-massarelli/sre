#include "rendering/materials/ShadowMapMaterial.h"
#include "Engine.h"

ShadowMapMaterial::ShadowMapMaterial()
    : Material{ "shaders/shadowMapVS.glsl", "shaders/shadowMapFS.glsl" } {
    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void ShadowMapMaterial::use() {
    shader.use();

    glDisable(GL_CULL_FACE);
}

void ShadowMapMaterial::after() {
    glEnable(GL_CULL_FACE);

    shader.stop();
}
