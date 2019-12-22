#include "rendering/effects/FXAA.h"
#include "Engine.h"
#include "cameras/CameraComponent.h"
#include <glm/glm.hpp>


FXAA::FXAA() : Effect{ "fxaa", "effects/fxaa.glsl" }
{
}

void FXAA::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.setVec2("pixelSize", 1.0f / glm::vec2{ Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight() });
	postProcessingShader.setFloat("far", Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getFarPlaneDistance());
	postProcessingShader.setFloat("near", Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getNearPlaneDistance());
}

FXAA::~FXAA()
{
}