#include "rendering/effects/GaussianBlur.h"
#include "Engine.h"

void GaussianBlur::initRenderTargets(RenderTarget* resultBuffer) {
#ifdef SRE_DEBUG
    assert(resultBuffer != nullptr && resultBuffer->isValid());
#endif

    mResultBuffer = resultBuffer;
     
    const auto width = resultBuffer->getWidth();
    const auto height = resultBuffer->getHeight();
    auto settings = resultBuffer->getColorBuffer()->getSettings();
    settings.appearanceOptions.hasMipmap = false; // there is no need to use mipmaps here
    mHorizontalBlurred
        = std::make_unique<Texture>(Texture::load(nullptr, static_cast<std::int32_t>(width), static_cast<std::int32_t>(height), settings));
    mHorizontalTarget = RenderTarget{ mHorizontalBlurred.get(), nullptr };

    hBlur = Shader::loadFromFile(
        std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/gaussianBlurFS.glsl" }, false);
    vBlur = Shader::loadFromFile(
        std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/gaussianBlurFS.glsl" }, false);

    {
        ShaderScopedUsage useShader{ hBlur };
        hBlur.setVec2("direction", glm::vec2{ 0.0f, 1.0f });
    }

    {
        ShaderScopedUsage useShader{ vBlur };
        vBlur.setVec2("direction", glm::vec2{ 1.0f, 0.0f });
    }
}

GaussianBlur::GaussianBlur(float scaleFactor, Texture::Settings outputSettings) {
    outputSettings.appearanceOptions.hasMipmap = false; // there is no need to use mipmaps here
    outputSettings.appearanceOptions.minFilter = GL_LINEAR; // force linear filtering so that it looks good when scaleFactor < 1
    outputSettings.appearanceOptions.magFilter = GL_LINEAR;

    const auto width = Engine::renderSys.getScreenWidth() * scaleFactor;
    const auto height = Engine::renderSys.getScreenHeight() * scaleFactor;
    
    mCombinedBlurred
        = std::make_unique<Texture>(Texture::load(nullptr, static_cast<std::int32_t>(width), static_cast<std::int32_t>(height), outputSettings));
    mCombinedTarget = RenderTarget{ mCombinedBlurred.get(), nullptr };

    initRenderTargets(&mCombinedTarget);
}

GaussianBlur::GaussianBlur(RenderTarget* renderTarget) {
    initRenderTargets(renderTarget);
}

const Texture& GaussianBlur::getBlurred(const Texture& src, int iterations) {
    if (iterations == 0) {
        return src;
    }
    Texture origin = src;
    for (int i = 0; i < iterations * 2; i++) {
        if (i % 2 == 0) {
            Engine::renderSys.copyTexture(origin, mHorizontalTarget, hBlur, false);
        } else {
            Engine::renderSys.copyTexture(*mHorizontalBlurred.get(), *mResultBuffer, vBlur, false);
        }

        origin = *mResultBuffer->getColorBuffer();
    }

    return *mResultBuffer->getColorBuffer();
}
