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

	bool mNeedsUpdate = false;
	int mKernelSize = 12;
	float mRadius = 2.5f;
	float mDarkenFactor = 2.2f;
	int mBlurSize = 2;

	void createSamples(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine);

	void createNoiseTexture(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine);

public:
	SSAO();

	virtual void onSetup(Shader& postProcessingShader) override;

	virtual void update(Shader& postProcessingShader) override;

	/**
	 * Sets the size of the kernel used to sample for ssao.
	 * In order to detect whether a part of the image is occluded a number
	 * of vectors sample its surroundings. The number of vectors is determined
	 * by the size of the kernel.
	 * A higher value means higher precision but it may impact performance.
	 * @param size the size of the sampling kernel, max is 64.
	 */
	void setKernelSize(int size);

	/**
	 * @return the kernel size
	 */
	int getKernelSize() const;

	/**
	 * Sets the size of the sampling vectors.
	 * Each sampling vector starts at the current pixel and moves with
	 * a length equal to radius in a random direction.
	 * A higher value can be used in conjunction with a lower kernel size but the
	 * result might be a nosier.
	 * @param radius the length of vectors
	 */
	void setRadius(float radius);

	/**
	 * @return the radius
	 */
	float getRadius() const;

	/**
	 * Sets how much the occluded areas should be darkened.
	 * @param factor the darken factor.
	 */
	void setDarkenFactor(float factor);

	/**
	 * @return the darken factor
	 */
	float getDarkenFactor() const;

	/**
	 * Sets the blur size.
	 * To achieve a more realistic result, a blur effect is applied to the ambient occlusion.
	 * The size of the blur effect determines how blurry the occlusion will be.
	 * @param size the size of the blur effect.
	 */
	void setBlurSize(int size);

	/**
	 * @return the blur size
	 */
	int getBlurSize() const;

	~SSAO();
};

