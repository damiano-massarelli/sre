#include "Effect.h"


Effect::Effect(const std::string& effectPath) : mEffectPath{effectPath}
{

}

const std::string& Effect::getEffectPath() const
{
	return mEffectPath;
}
