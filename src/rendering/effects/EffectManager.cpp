#include "EffectManager.h"
#include <algorithm>
#include <sstream>

#include <iostream>

void EffectManager::createShader()
{
	std::vector<std::string> shaderPaths;
	shaderPaths.push_back("effects/__postProcessingDeclFS.glsl");
	std::transform(mEffects.begin(), mEffects.end(), std::back_inserter(shaderPaths), [](auto& elem) { return elem->getEffectPath(); });
	shaderPaths.push_back("effects/__postProcessingMainFS.glsl");

	std::vector<std::string> code;
	std::transform(shaderPaths.begin(), shaderPaths.end(), std::back_inserter(code), [](const auto& path) { return Shader::sourceFromFile(path); });

	if (mEffects.size() != 0) {
		std::stringstream effectCalls;
		for (const auto& effect : mEffects)
			effectCalls << effect->getName() << "(color);\n";


		code.push_back(effectCalls.str());
	}
	//code.push_back(Shader::sourceFromFile("effects/__postProcessingEndFS.glsl"));

	mPostProcessingShader = Shader::fromCode({ Shader::sourceFromFile("effects/__postProcessingEffectVS.glsl") },
		{},
		code);
}

void EffectManager::addEffect(const std::shared_ptr<Effect>& effect)
{
	mEffects.push_back(effect);

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

