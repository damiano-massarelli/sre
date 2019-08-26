#include "GodRays.h"
#include "Engine.h"

GodRays::GodRays() : Effect{ "godrays", "effets/godrays.glsl" }
{
	mBlurredTarget.create(Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), true, false);
}

void GodRays::onSetup(Shader& postProcessingShader)
{
	mBlurredTexture = Engine::renderSys.effectManager.getTexture();
}

void GodRays::update(Shader& postProcessingShader)
{
	Engine::renderSys.copyTexture(Engine::renderSys.deferredRenderingFBO.getDepthBuffer(), mBlurredTarget, )
}

GodRays::~GodRays()
{
	Engine::renderSys.effectManager.releaseTexture(mBlurredTexture);
}