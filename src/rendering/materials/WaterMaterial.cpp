#include "WaterMaterial.h"
#include "RenderSystem.h"
#include "Engine.h"
#include "RenderPhase.h"
#include <glm/glm.hpp>

WaterMaterial::WaterMaterial(float waterY)
	: Material{"shaders/phongVS.glsl", "shaders/waterFS.glsl"}, mWaterY{ waterY }
{
	target.create(640, 480);

	// don't render when rendering for water or shadows
	unSupportedRenderPhases |= RenderPhase::ALL & ~RenderPhase::FORWARD_RENDERING;

	mEventCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
	shader.use();
	shader.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);
}

void WaterMaterial::onEvent(SDL_Event e)
{
	Engine::renderSys.enableClipPlane();
	Engine::renderSys.setClipPlane(glm::vec4{ 0, 1, 0, -mWaterY });
	Engine::renderSys.renderScene(&target, RenderPhase::WATER);
	Engine::renderSys.disableClipPlane();
}

void WaterMaterial::use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, target.getColorBuffer().getId());
	shader.use();
}

void WaterMaterial::after()
{
	
}