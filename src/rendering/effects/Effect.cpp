#include "rendering/effects/Effect.h"
#include "Engine.h"

Effect::Effect(const Settings& settings) {
    mOutput = Texture::load(nullptr,
        static_cast<int>(settings.outputSize.x),
        static_cast<int>(settings.outputSize.y),
        settings.outputSettings);

    mOutputTarget = RenderTarget(&mOutput, nullptr);

    mPostProcessingShader
        = Shader::loadFromFile(std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { settings.effectPath });
}

Effect::Effect(const std::string& name, const std::string& effectPath)
    : Effect{ Settings{ name,
        effectPath,
        glm::vec2{ Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight() },
        GBuffer::DIFFUSE_BUFFER_SETTINGS } } { }

const std::string& Effect::getEffectPath() const {
    return mEffectPath;
}

const std::string& Effect::getName() const {
    return mName;
}

const Texture& Effect::getOutput() const {
    return mOutput;
}

void Effect::applyEffect(const Texture& input, const RenderTarget* dst) {
    dst = (dst != nullptr && dst->isValid()) ? dst : &mOutputTarget;
    Engine::renderSys.copyTexture(input, *dst, mPostProcessingShader);
}

void Effect::applyEffect(const std::vector<std::reference_wrapper<const Texture>>& inputs, const RenderTarget* dst) {
    dst = (dst != nullptr && dst->isValid()) ? dst : &mOutputTarget;
    Engine::renderSys.copyTexture(inputs, *dst, mPostProcessingShader);
}
