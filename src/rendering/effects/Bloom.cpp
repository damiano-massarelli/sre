#include "Bloom.h"
#include "Engine.h"

Bloom::Bloom(float scaleFactor) : Effect{ "bloom", "effects/bloom.glsl" }, mGaussianBlur{ scaleFactor }
{
	mBloom = Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), GL_REPEAT, GL_REPEAT, false, GL_RGBA, GL_FLOAT, GL_RGBA16F);

	mTarget.createWith(mBloom, Texture{});

	mBloomExtractor = Shader::loadFromFile(std::vector<std::string>{ "effects/bloomExtractVS.glsl" }, {}, { "effects/bloomExtractFS.glsl" });

	mBlurredTexture = Engine::renderSys.effectManager.getTexture();
}

void Bloom::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.setInt("bloomTexture", mBlurredTexture);
}

void Bloom::update(Shader& postProcessingShader)
{
	if (mNeedsUpdate) {
		postProcessingShader.use();
		postProcessingShader.setFloat("_bloom_bloomFactor", mBloomFactor);
		mNeedsUpdate = false;
	}
	RenderSystem& rsys = Engine::renderSys;

 	Engine::renderSys.copyTexture(rsys.effectTarget.getColorBuffer(), mTarget, mBloomExtractor);
	
	const auto& blurred = mGaussianBlur.getBlurred(mBloom, 1);

	glActiveTexture(GL_TEXTURE0 + mBlurredTexture);
	glBindTexture(GL_TEXTURE_2D, blurred.getId());
}

void Bloom::setBloomFactor(float bloomFactor)
{
	mNeedsUpdate = true;
	mBloomFactor = bloomFactor;
}

float Bloom::getBloomFactor() const
{
	return mBloomFactor;
}

Bloom::~Bloom()
{
	mBloomExtractor = Shader{};
	Engine::renderSys.effectManager.releaseTexture(mBlurredTexture);
}