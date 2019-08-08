#pragma once
#include "Effect.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <random>

/**
 * Screen space ambient occlusion effect.
 */
class SSAO :
	public Effect
{
private:
	int mSSAOTextureIndex = -1;
	int mNoiseTextureIndex = -1;
	int mNormalTextureIndex = -1;

	std::vector<glm::vec3> mSSAOSamples;

	Texture mNoiseTexture;

	RenderTarget mSSAOCreationTarget;

	Shader mSSAOCreationShader;

	void createSamples(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine);

	void createNoiseTexture(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine);

public:
	SSAO();

	virtual void onSetup(Shader& postProcessingShader) override;

	virtual void update(Shader& postProcessingShader) override;

	~SSAO();
};

