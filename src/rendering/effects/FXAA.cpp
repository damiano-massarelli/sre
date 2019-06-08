#include "FXAA.h"
#include "Engine.h"
#include <glm/glm.hpp>

FXAA::FXAA() : Effect{ "fxaa", "effects/fxaaFS.glsl" }
{
}

void FXAA::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.setVec2("pixelSize", glm::vec2{ Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight() });
}

FXAA::~FXAA()
{
}