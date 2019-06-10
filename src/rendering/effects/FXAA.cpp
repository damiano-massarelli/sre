#include "FXAA.h"
#include "Engine.h"
#include <glm/glm.hpp>

#include <iostream>

FXAA::FXAA() : Effect{ "fxaa", "effects/fxaa.glsl" }
{
}

void FXAA::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.setVec2("pixelSize", 1.0f / glm::vec2{ Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight() });
	postProcessingShader.setFloat("far", Engine::renderSys.getFarPlane());
	postProcessingShader.setFloat("near", Engine::renderSys.getNearPlane());
}

FXAA::~FXAA()
{
}