#pragma once
#include "rendering/effects/Effect.h"
#include <cstdint>
#include <glm/glm.hpp>

class MotionBlur : public Effect {
private:
    glm::mat4 mPrevProjViewMatrix = glm::mat4{ 1.F };
    std::int32_t mPrevProjViewMatrixLocation = 0;
    std::int32_t mCurrentProjViewMatrixLocation = 0;

    float mBlurFactor = 100.0f;
    bool mBlurNeedsUpdate = false;

public:
    MotionBlur();

    void setBlurFactor(float blurFactor);

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;

    virtual void applyEffect(const Texture& input, const RenderTarget* dst) override;
};
