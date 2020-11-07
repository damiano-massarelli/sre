#include "rendering/effects/GodRays.h"
#include "Engine.h"
#include "rendering/light/Light.h"
#include "rendering/materials/Shader.h"

GodRays::GodRays()
    : Effect{ "godRays", "effects/godRays.glsl" } { }

void GodRays::setDensity(float density) {
    mDensity = density;
    mNeedUpdate = true;
}

float GodRays::getDensity() const {
    return mDensity;
}

void GodRays::setDecayRatio(float ratio) {
    mDecayRatio = ratio;
    mNeedUpdate = true;
}

float GodRays::getDecayRatio() const {
    return mDecayRatio;
}

void GodRays::setWeight(float weight) {
    mWeight = weight;
    mNeedUpdate = true;
}

float GodRays::getWeight() const {
    return mWeight;
}

void GodRays::setNumSamples(std::int32_t samples) {
    mNumSamples = samples;
    mNeedUpdate = true;
}

std::int32_t GodRays::getNumSamples() const {
    return mNumSamples;
}

void GodRays::setLight(const GameObjectEH& light) {
    mLight = light;
    mLightComponent = light->getComponent<Light>();

#ifdef SRE_DEBUG
    assert(mLight.isValid() && !mLightComponent.expired());
#endif  // SRE_DEBUG
}

const GameObjectEH& GodRays::getLight() const {
    return mLight;
}

void GodRays::onSetup(Shader& postProcessingShader) {
    mLightScreenPosLocation = postProcessingShader.getLocationOf("_gr_lightScreenPos");
    mRadiusLocation = postProcessingShader.getLocationOf("_gr_radius");
    mLightColorLocation = postProcessingShader.getLocationOf("_gr_lightColor");
}

void GodRays::update(Shader& postProcessingShader) {
#ifdef SRE_DEBUG
    assert(mLight.isValid() && !mLightComponent.expired());
#endif  // SRE_DEBUG

    auto& rsys = Engine::renderSys;

    // calculate screen-space light position
    auto& lightPosition = mLight->transform.getPosition();
    auto& cameraTransform = rsys.getCamera()->transform;

    glm::vec4 transformedPosition
        = rsys.getProjectionMatrix() * rsys.getViewMatrix(cameraTransform) * glm::vec4{ lightPosition, 1.0f };
    glm::vec3 projectedPos = glm::vec3{ transformedPosition } / transformedPosition.w;

    // calculate radius projecting it
    auto projectedRadiusDisplacement = rsys.getProjectionMatrix()
        * glm::vec4{ lightRadius, 0.0f, glm::distance(lightPosition, cameraTransform.getPosition()), 0.0f };

    float projectedRadius = lightRadius;
    if (isPerspective) {
        projectedRadius = glm::abs(projectedRadiusDisplacement.x / projectedRadiusDisplacement.w);
    }

    {
        ShaderScopedUsage useShader{ postProcessingShader };

        postProcessingShader.setVec3(mLightScreenPosLocation, projectedPos);
        postProcessingShader.setFloat(mRadiusLocation, projectedRadius);
        postProcessingShader.setVec3(mLightColorLocation, mLightComponent.lock()->diffuseColor);
        if (mNeedUpdate) {
            postProcessingShader.setFloat("_gr_density", mDensity);
            postProcessingShader.setFloat("_gr_decayRatio", mDecayRatio);
            postProcessingShader.setFloat("_gr_weight", mWeight);
            postProcessingShader.setInt("_gr_samples", mNumSamples);

            mNeedUpdate = false;
        }
    }
}
