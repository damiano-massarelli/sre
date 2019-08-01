#pragma once
#include "Effect.h"
#include "Shader.h"
#include <vector>
#include <utility>
#include <string.h>
#include <memory.h>
#include <set>

class EffectManager
{
friend class RenderSystem;

private:
	bool mEnabled = false;

	Shader mPostProcessingShader;

	std::set<int> mInUseTextures;

	std::vector<std::shared_ptr<Effect>> mEffects;

	void createShader(std::vector<std::shared_ptr<Effect>> effects);

public:
	EffectManager();

	EffectManager(const EffectManager& em) = delete;

	EffectManager& operator=(const EffectManager& em) = delete;

	void init();

	void addEffect(const std::shared_ptr<Effect>& effect);

	void enableEffects();

	void disableEffects();

	int getTexture();

	void releaseTexture(int texture);

	void update();

	void cleanUp();
};

