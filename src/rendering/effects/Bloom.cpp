#include "rendering/effects/Bloom.h"
#include "Engine.h"

Bloom::Bloom(float scaleFactor)
    : Effect{ "bloom", "effects/bloom.glsl" }
    , mGaussianBlur{ scaleFactor } {
    Texture::Settings loadOptions;
    loadOptions.internalFormat = GL_RGB16F;
    loadOptions.appearanceOptions.createMipmap = false;
    loadOptions.appearanceOptions.wrapS = GL_REPEAT;
    loadOptions.appearanceOptions.wrapT = GL_REPEAT;

    mBloom
        = Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), loadOptions);

    mTarget = RenderTarget{ &mBloom, nullptr };

    mBloomExtractor = Shader::loadFromFile(
        std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/bloomExtractFS.glsl" });

    mBlurredTexture = Engine::renderSys.effectManager.getTexture();
}

void Bloom::onSetup(Shader& postProcessingShader) {
    postProcessingShader.setInt("bloomTexture", mBlurredTexture);
}

void Bloom::update(Shader& postProcessingShader) {
    if (mNeedsUpdate) {
        ShaderScopedUsage useShader{ postProcessingShader };
        postProcessingShader.setFloat("_bloom_bloomFactor", mBloomFactor);
        mNeedsUpdate = false;
    }
    RenderSystem& rsys = Engine::renderSys;

    Engine::renderSys.copyTexture(*(rsys.lightPassRenderTarget.getColorBuffer()), mTarget, mBloomExtractor);

    const auto& blurred = mGaussianBlur.getBlurred(mBloom, 1);

    glActiveTexture(GL_TEXTURE0 + mBlurredTexture);
    glBindTexture(GL_TEXTURE_2D, blurred.getId());
}

void Bloom::setBloomFactor(float bloomFactor) {
    mNeedsUpdate = true;
    mBloomFactor = bloomFactor;
}

float Bloom::getBloomFactor() const {
    return mBloomFactor;
}

Bloom::~Bloom() {
    mBloomExtractor = Shader{};
    Engine::renderSys.effectManager.releaseTexture(mBlurredTexture);
}