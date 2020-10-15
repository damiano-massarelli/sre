#include "rendering/effects/GodRays.h"
#include "Engine.h"

GodRays::GodRays(float scaleFactor)
    : Effect{ "godRays", "effects/godRays.glsl" } {
    auto width = static_cast<std::uint32_t>(Engine::renderSys.getScreenWidth() * scaleFactor);
    auto height = static_cast<std::uint32_t>(Engine::renderSys.getScreenHeight() * scaleFactor);

    mOcclusionTarget.create(width, height, true, false);

    mOcclusionCreator = Shader::loadFromFile(
        std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/godraysCreateFS.glsl" });

    {
        ShaderScopedUsage useShader{ mOcclusionCreator };
        mLightScreenPosLocation = mOcclusionCreator.getLocationOf("lightScreenPos");
        mRadiusLocation = mOcclusionCreator.getLocationOf("radius");

        mBlurredTexture = Engine::renderSys.effectManager.getTexture();
    }
}

void GodRays::setDensity(float density) {
    mDensity = density;
    mNeedUpdate = true;
}

float GodRays::getDensity() const { return mDensity; }

void GodRays::setDecayRatio(float ratio) {
    mDecayRatio = ratio;
    mNeedUpdate = true;
}

float GodRays::getDecayRatio() const { return mDecayRatio; }

void GodRays::setWeight(float weight) {
    mWeight = weight;
    mNeedUpdate = true;
}

float GodRays::getWeight() const { return mWeight; }

void GodRays::onSetup(Shader& postProcessingShader) {
    postProcessingShader.setInt("_gr_raysTexture", mBlurredTexture);
    mLightScreenPosForRadialBlurLocation = postProcessingShader.getLocationOf("_gr_lightScreenPos");
}

void GodRays::update(Shader& postProcessingShader) {
    auto& rsys = Engine::renderSys;

    // calculate screen-space light position
    auto& lightPosition = light->transform.getPosition();
    auto& cameraTransform = rsys.getCamera()->transform;

    glm::vec4 transformedPosition
        = rsys.getProjectionMatrix() * rsys.getViewMatrix(cameraTransform) * glm::vec4{ lightPosition, 1.0f };
    glm::vec3 projectedPos = glm::vec3{ transformedPosition } / transformedPosition.w;

    // calculate radius projecting it
    auto projectedRadiusDisplacement = rsys.getProjectionMatrix()
        * glm::vec4{ lightRadius, 0.0f, glm::distance(lightPosition, cameraTransform.getPosition()), 0.0f };
    float projectedRadius = glm::abs(projectedRadiusDisplacement.x / projectedRadiusDisplacement.w);

    {
        ShaderScopedUsage useShader{ mOcclusionCreator };
        mOcclusionCreator.setVec3(mLightScreenPosLocation, projectedPos);
        mOcclusionCreator.setFloat(mRadiusLocation, projectedRadius);

        // create the radial blurred image
        rsys.copyTexture(Engine::renderSys.deferredRenderingFBO.getDepthBuffer(), mOcclusionTarget, mOcclusionCreator);
    }

    glActiveTexture(GL_TEXTURE0 + mBlurredTexture);
    glBindTexture(GL_TEXTURE_2D, mOcclusionTarget.getColorBuffer().getId());

    {
        ShaderScopedUsage useShader{ postProcessingShader };

        postProcessingShader.setVec3(mLightScreenPosForRadialBlurLocation, projectedPos);
        if (mNeedUpdate) {
            postProcessingShader.setFloat("_gr_density", mDensity);
            postProcessingShader.setFloat("_gr_decayRatio", mDecayRatio);
            postProcessingShader.setFloat("_gr_weight", mWeight);

            mNeedUpdate = false;
        }
    }
}

GodRays::~GodRays() { Engine::renderSys.effectManager.releaseTexture(mBlurredTexture); }