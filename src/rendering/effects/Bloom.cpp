#include "rendering/effects/Bloom.h"
#include "Engine.h"

Bloom::Bloom(float scaleFactor)
    : Effect{ "bloom", "effects/bloom.glsl" }
    , mGaussianBlur{ scaleFactor } {
    Texture::Settings loadOptions;
    loadOptions.internalFormat = GL_RGB16F;
    loadOptions.appearanceOptions.hasMipmap = false;
    loadOptions.appearanceOptions.wrapS = GL_REPEAT;
    loadOptions.appearanceOptions.wrapT = GL_REPEAT;

    mBloom
        = Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), loadOptions);
    mTarget = RenderTarget{ &mBloom, nullptr };

    mBloomExtractor = Shader::loadFromFile(
        std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/bloomExtractFS.glsl" });

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setInt("inputTexture", 0);
    mPostProcessingShader.setInt("bloomTexture", 1);
}

void Bloom::applyEffect(const Texture& input, const RenderTarget* dst) {
    Engine::renderSys.copyTexture(input, mTarget, mBloomExtractor);
    const auto& blurred = mGaussianBlur.getBlurred(mBloom, 1);
    Effect::applyEffect({ input, blurred }, dst);
}

void Bloom::setBloomFactor(float bloomFactor) {
    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setFloat("bloomFactor", mBloomFactor);
}

float Bloom::getBloomFactor() const {
    return mBloomFactor;
}

Bloom::~Bloom() {
    mBloomExtractor = Shader{};
}