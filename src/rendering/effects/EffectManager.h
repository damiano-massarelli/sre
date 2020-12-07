#pragma once
#include "rendering/effects/Effect.h"
#include "rendering/materials/Shader.h"
#include <memory.h>
#include <set>
#include <string.h>
#include <utility>
#include <vector>
#include <algorithm>

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

    [[nodiscard]] bool hasEffect(std::shared_ptr<Effect> effect) const;

    template <typename T>
    [[nodiscard]] bool hasEffect() const {
        return std::find_if(mEffects.begin(), mEffects.end(), [](auto& effect) {
            return dynamic_cast<T*>(effect.get()) != nullptr;
        }) != mEffects.end();
    }

    void enableEffects();

    void disableEffects();

    int getTexture();

    void releaseTexture(int texture);

    void update();

    void cleanUp();

    void shutdown();
};
