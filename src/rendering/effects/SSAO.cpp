#include "rendering/effects/SSAO.h"
#include "Engine.h"



SSAO::SSAO() 
	: Effect{"ssao", "effects/ssao.glsl"}
{
	// texture for ssao as an effect
	mSSAOTextureIndex = Engine::renderSys.effectManager.getTexture();
	mNoiseTextureIndex = Engine::renderSys.effectManager.getTexture();
	mNormalTextureIndex = Engine::renderSys.effectManager.getTexture();

	Texture::TextureLoadOptions loadOptions;
	loadOptions.dataPixelFormat = GL_RGBA;
	loadOptions.dataPixelType = GL_FLOAT;
	loadOptions.internalFormat = GL_RED;
	loadOptions.appearanceOptions.createMipmap = false;
	loadOptions.appearanceOptions.magFilter = GL_NEAREST;
	loadOptions.appearanceOptions.minFilter = GL_NEAREST;
	loadOptions.appearanceOptions.wrapS = GL_REPEAT;
	loadOptions.appearanceOptions.wrapT = GL_REPEAT;
	mSSAOCreationTarget.createWith(
		// only red component is used
		Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), loadOptions),
		Texture{}
	);


	std::uniform_real_distribution<float> dist;
	std::default_random_engine engine;

	createSamples(dist, engine);
	createNoiseTexture(dist, engine);

	mSSAOCreationShader = Shader::loadFromFile({ "effects/ssaoCreateVS.glsl" }, std::vector<std::string>{}, { "effects/ssaoCreateFS.glsl" }, false);

	{
		ShaderScopedUsage useShader{ mSSAOCreationShader };
		mSSAOCreationShader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
		mSSAOCreationShader.bindUniformBlock("Camera", Engine::renderSys.CAMERA_UNIFORM_BLOCK_INDEX);
		mSSAOCreationShader.setVec3Array("samples", mSSAOSamples);
		mSSAOCreationShader.setInt("NormalData", mNormalTextureIndex);
		mSSAOCreationShader.setInt("noise", mNoiseTextureIndex);
	}
}

void SSAO::createSamples(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine)
{
	const int numSamples = 64;
	mSSAOSamples.reserve(numSamples);

	for (int i = 0; i < 64; i++) {
		auto sample = glm::vec3{
			dist(engine) * 2.0f - 1.0f,
			dist(engine) * 2.0f - 1.0f,
			dist(engine)  // only on the positive direction of the hemisphere
		};

		sample = glm::normalize(sample);
		sample *= dist(engine);

		// make sure the majority of samples are close to the fragment
		float scale = i / 64.0f;
		scale = glm::mix(0.1f, 1.0f, scale * scale);

		sample *= scale;
		mSSAOSamples.push_back(sample);
	}
}

void SSAO::createNoiseTexture(std::uniform_real_distribution<float>& dist, std::default_random_engine& engine)
{
	std::vector<glm::vec3> noiseData;

	// resolution width and height
	const int res = 4;

	noiseData.reserve(res * res);

	for (int i = 0; i < res * res; ++i) {
		auto rotation = glm::vec3{
			dist(engine) * 2.0f - 1.0f,
			dist(engine) * 2.0f - 1.0f,
			0.0
		};

		noiseData.push_back(rotation);
	}

	Texture::TextureLoadOptions loadOptions;
	loadOptions.dataPixelType = GL_FLOAT;
	loadOptions.dataPixelFormat = GL_RGB;
	loadOptions.internalFormat = GL_RGB16F;
	loadOptions.appearanceOptions.wrapS = GL_REPEAT;
	loadOptions.appearanceOptions.wrapT = GL_REPEAT;
	loadOptions.appearanceOptions.createMipmap = false;
	mNoiseTexture = Texture::load(noiseData.data(), res, res, loadOptions);
}

void SSAO::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.setInt("_ssao_texture", mSSAOTextureIndex);
}

void SSAO::update(Shader& postProcessingShader)
{
	if (mNeedsUpdate) {
		ShaderScopedUsage useShader{ postProcessingShader };

		mNeedsUpdate = false;
		postProcessingShader.setFloat("_ssao_darkenFactor", mDarkenFactor);
		postProcessingShader.setInt("_ssao_blurSize", mBlurSize);
	}

	RenderSystem& rsys = Engine::renderSys;

	glActiveTexture(GL_TEXTURE0 + mNormalTextureIndex);
	glBindTexture(GL_TEXTURE_2D, rsys.deferredRenderingFBO.getNormalBuffer().getId());

	glActiveTexture(GL_TEXTURE0 + mNoiseTextureIndex);
	glBindTexture(GL_TEXTURE_2D, mNoiseTexture.getId());

	Engine::renderSys.copyTexture(rsys.deferredRenderingFBO.getPositionBuffer(), mSSAOCreationTarget, mSSAOCreationShader);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + mNormalTextureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);

	// bind the ssao texture
	glActiveTexture(GL_TEXTURE0 + mSSAOTextureIndex);
	glBindTexture(GL_TEXTURE_2D, mSSAOCreationTarget.getColorBuffer().getId());
}

void SSAO::setKernelSize(int size)
{
	if (size > 64) size = 64;

	mKernelSize = size;
	
	ShaderScopedUsage useShader{ mSSAOCreationShader };
	mSSAOCreationShader.setInt("kernelSize", size);
}

int SSAO::getKernelSize() const
{
	return mKernelSize;
}

void SSAO::setRadius(float radius)
{
	mRadius = radius;

	{
		ShaderScopedUsage usage{ mSSAOCreationShader };
		mSSAOCreationShader.setFloat("radius", radius);
	}
}

float SSAO::getRadius() const
{
	return mRadius;
}

void SSAO::setDarkenFactor(float factor)
{
	mDarkenFactor = factor;
	mNeedsUpdate = true;
}

float SSAO::getDarkenFactor() const
{
	return mDarkenFactor;
}

void SSAO::setBlurSize(int size)
{
	mBlurSize = size;
	mNeedsUpdate = true;
}

int SSAO::getBlurSize() const
{
	return mBlurSize;
}

SSAO::~SSAO()
{
	Engine::renderSys.effectManager.releaseTexture(mSSAOTextureIndex);
	Engine::renderSys.effectManager.releaseTexture(mNoiseTextureIndex);
	Engine::renderSys.effectManager.releaseTexture(mNormalTextureIndex);
}
