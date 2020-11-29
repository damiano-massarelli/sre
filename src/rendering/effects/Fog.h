#pragma once

#include "rendering/RenderTarget.h"
#include "rendering/effects/Effect.h"
#include "rendering/materials/Shader.h"
#include "rendering/materials/Texture.h"


/**
 *  Fog effect.
 */
class Fog : public Effect {
private:
    Shader mPostProcessingShader;

    glm::vec3 mColor;
    float mStartDistance = 0.f;
    float mRapidity = 1.f;

public:
    Fog();

    virtual void onSetup(Shader& postProcessingShader) override;

    virtual void update(Shader& postProcessingShader) override;

    ~Fog();

    // Setters
    void setColor(const glm::vec3& color);
    void setStartDistance(float distance);
    void setRapidity(float rapidity);

    // Getter
    glm::vec3 getColor() const { return mColor; }
    float getStartDistance() const { return mStartDistance; }
    float getRapidity() const { return mRapidity; }
};
