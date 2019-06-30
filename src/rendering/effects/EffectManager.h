#pragma once
#include "Effect.h"
#include "Shader.h"
#include <vector>
#include <utility>
#include <string.h>
#include <memory.h>

class EffectManager
{
friend class RenderSystem;

private:
	bool mEnabled = false;

	Shader mPostProcessingShader;

	std::vector<std::shared_ptr<Effect>> mEffects;

	void createShader();

public:
	EffectManager() = default;

	void addEffect(const std::shared_ptr<Effect>& effect);

	void enableEffects();

	void disableEffects();

	void cleanUp();
};

