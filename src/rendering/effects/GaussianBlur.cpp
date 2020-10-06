#include "rendering/effects/GaussianBlur.h"
#include "Engine.h"

GaussianBlur::GaussianBlur(float scaleFactor)
{
	Texture::Settings loadOptions;
	loadOptions.internalFormat = GL_RGB16F;
	loadOptions.appearanceOptions.createMipmap = false;

	auto width = Engine::renderSys.getScreenWidth() * scaleFactor;
	auto height = Engine::renderSys.getScreenHeight() * scaleFactor;
	mBlurred = Texture::load(nullptr, static_cast<std::int32_t>(width), static_cast<std::int32_t>(height), loadOptions);
	mTarget.createWith(mBlurred, Texture{});

	hBlur = Shader::loadFromFile(std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/gaussianBlurFS.glsl" }, false);
	vBlur = Shader::loadFromFile(std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/gaussianBlurFS.glsl" }, false);

	{
		ShaderScopedUsage useShader{ hBlur };
		hBlur.setVec2("direction", glm::vec2{ 0.0f, 1.0f });
	}

	{
		ShaderScopedUsage useShader{ vBlur };
		vBlur.setVec2("direction", glm::vec2{ 1.0f, 0.0f });
	}
}

const Texture& GaussianBlur::getBlurred(const Texture& src, int iterations)
{
	if (iterations == 0) {
		return src;
	}
	Texture origin = src;
	for (int i = 0; i < iterations * 2; i++) {
		if (i % 2 == 0) {
			Engine::renderSys.copyTexture(origin, mTarget, hBlur, false);
		}
		else {
			Engine::renderSys.copyTexture(origin, mTarget, vBlur, false);
		}

		origin = mBlurred;
	}

	return mBlurred;
}

GaussianBlur::~GaussianBlur()
{

}
