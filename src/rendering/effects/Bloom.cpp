#include "Bloom.h"
#include "Engine.h"

Bloom::Bloom() : Effect{ "bloom", "effects/bloom.glsl" }
{
	mBloom = Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), GL_REPEAT, GL_REPEAT, false, GL_RGBA);

	mTarget.createWith(mBloom, Texture{});

	mBloomExtractor = Shader::loadFromFile(std::vector<std::string>{ "effects/bloomExtractVS.glsl" }, {}, { "effects/bloomExtractFS.glsl" });
}

void Bloom::onSetup(Shader& postProcessingShader)
{
	postProcessingShader.setInt("bloomTexture", 2);
}

#include <iostream>
void Bloom::update(Shader& postProcessingShader)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
		std::cout << "copy " << error << "\n";
	RenderSystem& rsys = Engine::renderSys;

 	Engine::renderSys.copyTexture(rsys.effectTarget.getColorBuffer(), mTarget, mBloomExtractor);
	

	const auto& blurred = mGaussianBlur.getBlurred(mBloom, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blurred.getId());
}

Bloom::~Bloom()
{
	mBloomExtractor = Shader{};
}