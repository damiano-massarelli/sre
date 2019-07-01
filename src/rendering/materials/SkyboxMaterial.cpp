#include "SkyboxMaterial.h"
#include "Engine.h"
#include <limits>

SkyboxMaterial::SkyboxMaterial(const Texture& cubemap) : 
	Material{ std::vector<std::string>{"shaders/skyboxVS.glsl"},
		    {},
			{"shaders/FogCalculation.glsl", "shaders/skyboxFS.glsl"} }, mCubemap{ cubemap }
{
	// do not render during shadow mapping
	supportedRenderPhases = RenderPhase::FORWARD_RENDERING;

    shader.use();
	shader.bindUniformBlock("Fog", RenderSystem::FOG_UNIFORM_BLOCK_INDEX);
    shader.setInt("cubemap", 0);
}

void SkyboxMaterial::use()
{
     shader.use();

     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemap.getId());

     // because it is the last thing being rendered
     // with the lowest value of depth
     glDepthFunc(GL_LEQUAL);

     glDisable(GL_CULL_FACE);
}

void SkyboxMaterial::after()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
}

float SkyboxMaterial::renderOrder(const glm::vec3& position)
{
    // be the last, please
    return -std::numeric_limits<float>::infinity();
}
