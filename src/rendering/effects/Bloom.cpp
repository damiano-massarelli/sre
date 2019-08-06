#include "Bloom.h"
#include "Engine.h"

Bloom::Bloom() : Effect{ "bloom", "effects/bloom.glsl" }, mGaussianBlur{ 0.75f }
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
	RenderSystem& rsys = Engine::renderSys;

 	Engine::renderSys.copyTexture(rsys.effectTarget.getColorBuffer(), mTarget, mBloomExtractor);
	
	const auto& blurred = mGaussianBlur.getBlurred(mBloom, 1);

	glActiveTexture(GL_TEXTURE0 + mBlurredTexture);
	glBindTexture(GL_TEXTURE_2D, blurred.getId());
}

Bloom::~Bloom()
{
	mBloomExtractor = Shader{};
	Engine::renderSys.effectManager.releaseTexture(mBlurredTexture);
}