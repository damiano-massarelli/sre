#include "rendering/materials/SkyboxMaterial.h"
#include "Engine.h"
#include <limits>

SkyboxMaterial::SkyboxMaterial(const Texture& cubemap)
    : Material{ std::vector<std::string>{ "shaders/skyboxVS.glsl" },
        {},
        { "shaders/FogCalculation.glsl", "shaders/skyboxFS.glsl" } }
    , mCubemap{ cubemap } {
    // do not render during shadow mapping
    unSupportedRenderPhases = RenderDomain::SHADOW_MAPPING | RenderDomain::PBR;

    ShaderScopedUsage useShader{ shader };
    shader.bindUniformBlock("Fog", RenderSystem::FOG_UNIFORM_BLOCK_INDEX);
    shader.setInt("cubemap", 0);
}

void SkyboxMaterial::use() {
    shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemap.getId());

    // because it is the last thing being rendered
    // with the lowest value of depth
    glDepthFunc(GL_LEQUAL);

    glDisable(GL_CULL_FACE);
}

void SkyboxMaterial::after() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);

    shader.stop();
}

float SkyboxMaterial::renderOrder(const glm::vec3& position) {
    // be the last, please
    return -std::numeric_limits<float>::infinity();
}

std::size_t SkyboxMaterial::hash() const {
    return Material::hash() + mCubemap.getId();
}

bool SkyboxMaterial::equalsTo(const Material* rhs) const {
    if (shader.getId() != rhs->shader.getId())
        return false;

    auto other = static_cast<const SkyboxMaterial*>(rhs);

    return Material::equalsTo(rhs) && mCubemap.getId() == other->mCubemap.getId();
}
