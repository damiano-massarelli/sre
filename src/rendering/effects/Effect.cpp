#include "rendering/effects/Effect.h"


Effect::Effect(const std::string& name, const std::string& effectPath) : mName{ name }, mEffectPath { effectPath }
{

}

const std::string& Effect::getEffectPath() const
{
	return mEffectPath;
}

const std::string& Effect::getName() const
{
	return mName;
}
