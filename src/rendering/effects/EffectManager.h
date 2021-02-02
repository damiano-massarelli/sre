#pragma once
#include "rendering/effects/Effect.h"
#include "rendering/materials/Shader.h"
#include <algorithm>
#include <memory.h>
#include <set>
#include <string.h>
#include <utility>
#include <vector>

class EffectManager {
    friend class RenderSystem;

private:
    bool mEnabled = false;

    std::vector<std::shared_ptr<Effect>> mEffects;

    void setupEffects();

public:
    EffectManager() = default;

    EffectManager(const EffectManager& em) = delete;

    EffectManager& operator=(const EffectManager& em) = delete;

    void addEffect(const std::shared_ptr<Effect>& effect);

    void removeEffect(std::shared_ptr<Effect> effect);

    bool hasEffect(std::shared_ptr<Effect> effect) const;

    template <typename T> bool hasEffect() const {
        return std::find_if(mEffects.begin(), mEffects.end(), [](auto& effect) {
            return dynamic_cast<T*>(effect.get()) != nullptr;
        }) != mEffects.end();
    }

    bool addEffectBefore(std::shared_ptr<Effect> effect, std::shared_ptr<Effect> next);

    template <typename Next> bool addEffectBefore(std::shared_ptr<Effect> effect) {
        const auto found = std::find_if(mEffects.cbegin(), mEffects.cend(), [](auto& effect) {
            return dynamic_cast<Next*>(effect.get()) != nullptr;
        });

        if (found == mEffects.cend()) {
            return false;
        }

        mEffects.insert(found, effect);
        setupEffects();
        return true;
    }

    void enableEffects();

    void disableEffects();

    const Texture* renderEffects(const Texture& input, const RenderTarget* dst = nullptr);

    bool hasEffects() const;

    bool isEnabled() const;

    void cleanUp();

    void shutdown();
};
