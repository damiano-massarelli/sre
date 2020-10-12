#include "SSR.h"
#include "Engine.h"
#include "rendering/RenderSystem.h"
#include "rendering/effects/EffectManager.h"
#include <cassert>

SSR::SSR()
    : Effect{ "ssr", "effects/ssr.glsl" } {
    mPositionTexture = Engine::renderSys.effectManager.getTexture();
    mNormalTexture = Engine::renderSys.effectManager.getTexture();
    mSpecularTexture = Engine::renderSys.effectManager.getTexture();
    assert(mPositionTexture != -1);
    assert(mNormalTexture != -1);
    assert(mSpecularTexture != -1);
}

void SSR::onSetup(Shader& postProcessingShader) {
    // ShaderScopedUsage useShader{ postProcessingShader };

    postProcessingShader.setInt("_ssr_position", mPositionTexture);
    postProcessingShader.setInt("_ssr_normals", mNormalTexture);
    postProcessingShader.setInt("_ssr_specular", mSpecularTexture);
}

void SSR::update(Shader& postProcessingShader) {
    ShaderScopedUsage useShader{ postProcessingShader };

    RenderSystem& rsys = Engine::renderSys;
    const glm::mat4 currentProjectViewMatrix
        = rsys.getProjectionMatrix() * rsys.getViewMatrix(rsys.getCamera()->transform);

    // TODO optimize me
    postProcessingShader.setVec3("_ssr_cameraPosition", rsys.getCamera()->transform.getPosition());
    postProcessingShader.setMat4("_ssr_projectionView", currentProjectViewMatrix);

    glActiveTexture(GL_TEXTURE0 + mPositionTexture);
    glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getPositionBuffer().getId());

    glActiveTexture(GL_TEXTURE0 + mNormalTexture);
    glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getNormalBuffer().getId());

    glActiveTexture(GL_TEXTURE0 + mSpecularTexture);
    glBindTexture(GL_TEXTURE_2D, Engine::renderSys.deferredRenderingFBO.getAdditionalBuffer().getId());
}
