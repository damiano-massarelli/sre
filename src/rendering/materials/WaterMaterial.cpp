#include "WaterMaterial.h"
#include "RenderSystem.h"
#include "Engine.h"
#include "RenderPhase.h"

#include <iostream>

WaterMaterial::WaterMaterial()
	: Material{"shaders/phongVS.glsl", "shaders/waterFS.glsl"}
{
	target.create(640, 480);

	// dont render when rendering for water or shadows
	supportedRenderPhases &= ~(RenderPhase::WATER | RenderPhase::SHADOW_MAPPING);

	mEventCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
	shader.use();
	shader.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);
}


void WaterMaterial::onEvent(SDL_Event e)
{
	Engine::renderSys.renderScene(RenderPhase::WATER, &target);
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
