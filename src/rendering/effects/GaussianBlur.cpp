#include "GaussianBlur.h"
#include "Engine.h"

GaussianBlur::GaussianBlur(float scaleFactor)
{
	auto width = Engine::renderSys.getScreenWidth() * scaleFactor;
	auto height = Engine::renderSys.getScreenHeight() * scaleFactor;
	mBlurred = Texture::load(nullptr, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), GL_REPEAT, GL_REPEAT, false, GL_RGBA);
	mTarget.createWith(mBlurred, Texture{});

	hBlur = Shader::loadFromFile(std::vector<std::string>{ "effects/gaussianBlurVS.glsl" }, {}, { "effects/gaussianBlurFS.glsl" }, false);
	vBlur = Shader::loadFromFile(std::vector<std::string>{ "effects/gaussianBlurVS.glsl" }, {}, { "effects/gaussianBlurFS.glsl" }, false);

	hBlur.use();
	hBlur.setVec2("direction", glm::vec2{ 0.0f, 1.0f });

	vBlur.use();
	vBlur.setVec2("direction", glm::vec2{ 1.0f, 0.0f });
}

const Texture& GaussianBlur::getBlurred(const Texture& src, int iterations)
{
	Texture origin = src;
	for (int i = 0; i < iterations * 2; i++) {
		if (i % 2 == 0)
			Engine::renderSys.copyTexture(origin, mTarget, hBlur, false);
		else
			Engine::renderSys.copyTexture(origin, mTarget, vBlur, false);

		if (i == 0)
			origin = mBlurred;
	}

	return mBlurred;
}

GaussianBlur::~GaussianBlur()
{

}
