#include "rendering/effects/EffectManager.h"
#include <sstream>

void EffectManager::createShader(std::vector<std::shared_ptr<Effect>> effects) {
    std::vector<std::string> shaderPaths;
    shaderPaths.push_back("effects/__postProcessingDeclFS.glsl");
    std::transform(effects.begin(), effects.end(), std::back_inserter(shaderPaths), [](auto& elem) {
        return elem->getEffectPath();
    });
    shaderPaths.push_back("effects/__postProcessingMainFS.glsl");

    std::vector<std::string> code;
    std::transform(shaderPaths.begin(), shaderPaths.end(), std::back_inserter(code), [](const auto& path) {
        return Shader::sourceFromFile(path) + "\n";
    });  // add \n to avoid problems with #line

    if (effects.size() != 0) {
        std::stringstream effectCalls;
        for (const auto& effect : effects)
            effectCalls << "color = " << effect->getName() << "(color);\n";

        code.push_back(effectCalls.str());
    }
    code.push_back(Shader::sourceFromFile("effects/__postProcessingEndFS.glsl"));

    mPostProcessingShader
        = Shader::fromCode({ Shader::sourceFromFile("effects/__postProcessingEffectVS.glsl") }, {}, code);

    {
        ShaderScopedUsage useShader{ mPostProcessingShader };

        /* Explicit use of getLocationOf to avoid uniform not found warnings */
        mPostProcessingShader.setInt(mPostProcessingShader.getLocationOf("screenTexture", false), 0);
        mPostProcessingShader.setInt(mPostProcessingShader.getLocationOf("depthTexture", false), 1);

        // call set up phase
        for (auto& effect : effects)
            effect->onSetup(mPostProcessingShader);
    }
}

EffectManager::EffectManager()
    : mInUseTextures{ 0, 1 }  // these textures are always in use for the effects
{ }

void EffectManager::init() {
    createShader({});
}

void EffectManager::addEffect(const std::shared_ptr<Effect>& effect) {
    mEffects.push_back(effect);
    if (mEnabled) {
        createShader(mEffects);
    }
}

void EffectManager::removeEffect(std::shared_ptr<Effect> effect)
{
    mEffects.erase(std::remove(mEffects.begin(), mEffects.end(), effect), mEffects.end());
    if (mEnabled) {
        createShader(mEffects);
    }
}

bool EffectManager::hasEffect(std::shared_ptr<Effect> effect) const {
    return std::find(mEffects.begin(), mEffects.end(), effect) != mEffects.end();
}

void EffectManager::enableEffects() {
    createShader(mEffects);
    mEnabled = true;
}

void EffectManager::disableEffects() {
    mEnabled = false;
    createShader({});  // TODO: why? - anyway, replace with init();
}

int EffectManager::getTexture() {
    int texture = -1;
    // todo replace 15 with the maximum number of available texture slots
    for (int i = 0; i < 15; ++i) {
        if (mInUseTextures.find(i) == mInUseTextures.end()) {
            texture = i;
            break;
        }
    }

    if (texture != -1) {
        mInUseTextures.insert(texture);
    }

    return texture;
}

void EffectManager::releaseTexture(int texture) {
    mInUseTextures.erase(texture);
}

void EffectManager::update() {
    if (!mEnabled)
        return;

    {
        ShaderScopedUsage useShader{ mPostProcessingShader };
        for (auto& effect : mEffects) {
            effect->update(mPostProcessingShader);
        }
    }
}

void EffectManager::cleanUp() {
    mEffects = {};

    init();
}

void EffectManager::shutdown() {
    mPostProcessingShader = Shader();
    mEffects = {};
}
