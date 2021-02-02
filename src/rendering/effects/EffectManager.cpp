#include "rendering/effects/EffectManager.h"

const Texture* EffectManager::renderEffects(const Texture& input, const RenderTarget* dst) {
    if (mEffects.empty()) {
        return &input;
    }

    const bool useDst = (dst != nullptr && dst->isValid());
    const Texture* currentInput = &input;
    for (auto i = 0; i < mEffects.size(); i++) {
        const RenderTarget* target = (i == mEffects.size() - 1 && useDst) ? dst : nullptr;
        mEffects[i]->update();
        mEffects[i]->applyEffect(*currentInput, target);
        currentInput = &(mEffects[i]->getOutput());
    }

    return useDst ? dst->getColorBuffer() : currentInput;
}

bool EffectManager::hasEffects() const {
    return !mEffects.empty();
}

bool EffectManager::isEnabled() const {
    return mEnabled;
}

void EffectManager::setupEffects() {
    for (auto effect : mEffects) {
        effect->onSetup();
    }
}

void EffectManager::addEffect(const std::shared_ptr<Effect>& effect) {
    mEffects.push_back(effect);
    setupEffects();
}

void EffectManager::removeEffect(std::shared_ptr<Effect> effect) {
    mEffects.erase(std::remove(mEffects.begin(), mEffects.end(), effect), mEffects.end());
}

bool EffectManager::hasEffect(std::shared_ptr<Effect> effect) const {
    return std::find(mEffects.begin(), mEffects.end(), effect) != mEffects.end();
}

bool EffectManager::addEffectBefore(std::shared_ptr<Effect> effect, std::shared_ptr<Effect> next) {
    const auto found = std::find(mEffects.cbegin(), mEffects.cend(), next);
    if (found == mEffects.cend()) {
        return false;
    }
    mEffects.insert(found, effect);
    setupEffects();
    return true;
}

void EffectManager::enableEffects() {
    mEnabled = true;
}

void EffectManager::disableEffects() {
    mEnabled = false;
}

void EffectManager::cleanUp() {
    mEffects = {};
}

void EffectManager::shutdown() {
    mEffects = {};
}
