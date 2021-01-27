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

    Shader mPostProcessingShader;

    std::set<int> mInUseTextures;

    std::vector<std::shared_ptr<Effect>> mEffects;

    void createShader(std::vector<std::shared_ptr<Effect>> effects);

public:
    EffectManager();

    EffectManager(const EffectManager& em) = delete;

    EffectManager& operator=(const EffectManager& em) = delete;

    void init();

    void addEffect(const std::shared_ptr<Effect>& effect);

    void removeEffect(std::shared_ptr<Effect> effect);

    bool hasEffect(std::shared_ptr<Effect> effect) const;

    template <typename T> bool hasEffect() const {
        return std::find_if(mEffects.begin(), mEffects.end(), [](auto& effect) {
            return dynamic_cast<T*>(effect.get()) != nullptr;
        }) != mEffects.end();
    }

    bool addEffectBefore(std::shared_ptr<Effect> effect, std::shared_ptr<Effect> next);

    void enableEffects();

    void disableEffects();

    int getTexture();

    void releaseTexture(int texture);

    void update();

    const Texture* renderEffects(const Texture& input, const RenderTarget* dst = nullptr);

    bool hasEffects() const;

    bool isEnabled() const;

    void cleanUp();

    void shutdown();
};
