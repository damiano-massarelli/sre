#include "EffectManager.h"
#include <algorithm>

void EffectManager::createShader()
{
	std::vector<std::string> shaders;
	std::transform(mEffects.begin(), mEffects.end(), shaders.begin(), [](auto& elem) { return elem.second->getEffectPath(); });
	shaders.insert(shaders.begin(), { "effects/__postProcessingStartFS.glsl" });
	shaders.push_back("effects/__postProcessingEndFS.glsl");

	mPostProcessingShader = Shader::load({ "effects/__postProcessingVS.glsl" }, shaders);
}

void EffectManager::addEffect(const std::string& name, const std::shared_ptr<Effect>& effect)
{
	mEffects.push_back(std::make_pair(name, effect));

	createShader();
}

void EffectManager::enableEffects()
{
	if (!mPostProcessingShader)
		createShader();
	mEnabled = true;
}

void EffectManager::disableEffects()
{
	mEnabled = false;
}

void EffectManager::cleanUp()
{
	mPostProcessingShader = Shader();
}

