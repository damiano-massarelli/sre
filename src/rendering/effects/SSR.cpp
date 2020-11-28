#include "SSR.h"
#include "Engine.h"
#include "rendering/RenderSystem.h"
#include "rendering/effects/EffectManager.h"
#include <cameras/CameraComponent.h>
#include <cassert>
#include <vector>

SSR::SSR()
    : Effect{ "ssr", "effects/ssr.glsl" }
    , mGaussianBlur{ 0.05F } {
    mPositionTexture = Engine::renderSys.effectManager.getTexture();
    mNormalTexture = Engine::renderSys.effectManager.getTexture();
    mMaterialTexture = Engine::renderSys.effectManager.getTexture();
    mBlurredSceneTexture = Engine::renderSys.effectManager.getTexture();
    assert(mPositionTexture != -1);
    assert(mNormalTexture != -1);
    assert(mMaterialTexture != -1);
    assert(mBlurredSceneTexture != -1);
}

void SSR::onSetup(Shader& postProcessingShader) {
    // ShaderScopedUsage useShader{ postProcessingShader };
    mPostProcessingShader = postProcessingShader;

    mPostProcessingShader.setInt("_ssr_position", mPositionTexture);
    mPostProcessingShader.setInt("_ssr_normals", mNormalTexture);
    mPostProcessingShader.setInt("_ssr_materialBuffer", mMaterialTexture);
    mPostProcessingShader.setInt("_ssr_blurredScene", mBlurredSceneTexture);

    // Parameter defaults
    mPostProcessingShader.setFloat("_ssr_rayMaxDistance", mMaxDistance);
    mPostProcessingShader.setFloat("_ssr_rayResolution", mResolution);
    mPostProcessingShader.setInt("_ssr_raySteps", mSteps);
    mPostProcessingShader.setFloat("_ssr_rayHitThreshold", mHitThreshold);

    // Cached shader locations
    mCameraPositionLocation = mPostProcessingShader.getLocationOf("_ssr_cameraPosition");
    mCameraDirectionLocation = mPostProcessingShader.getLocationOf("_ssr_cameraDirection");
    mProjectionViewLocation = mPostProcessingShader.getLocationOf("_ssr_projectionView");
    mFrustumPlanesLocation = mPostProcessingShader.getLocationOf("_ssr_frustumPlanes");
}

void SSR::update(Shader& postProcessingShader) {
    RenderSystem& renderSystem = Engine::renderSys;
    Texture blurred = mGaussianBlur.getBlurred(renderSystem.lightPassTarget, 4);

    ShaderScopedUsage useShader{ postProcessingShader };

    if (!renderSystem.lightPassTarget.getSettings().appearanceOptions.hasMipmap) {
        renderSystem.lightPassTarget.setRequireMipmap(true);
    }

    const glm::mat4 currentProjectViewMatrix
        = renderSystem.getProjectionMatrix() * renderSystem.getViewMatrix(renderSystem.getCamera()->transform);
    const auto cameraComponent = renderSystem.getCamera()->getComponent<CameraComponent>();
    const float near = cameraComponent->getNearPlaneDistance();
    const float far = cameraComponent->getFarPlaneDistance();

    if (near != mNearPlane) {
        mNearPlane = near;
        postProcessingShader.setFloat("_ssr_nearPlane", mNearPlane);
    }
    if (far != mFarPlane) {
        mFarPlane = far;
        postProcessingShader.setFloat("_ssr_farPlane", mFarPlane);
    }

    postProcessingShader.setVec3(mCameraPositionLocation, renderSystem.getCamera()->transform.getPosition());
    postProcessingShader.setVec3(mCameraDirectionLocation, renderSystem.getCamera()->transform.forward());
    postProcessingShader.setMat4(mProjectionViewLocation, currentProjectViewMatrix);

    // Set camera frustum planes
    std::vector<glm::vec4> frustumPlanes{6};
    std::array<Plane, 6> planes
        = Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getViewFrutsum().getPlanes();
    std::transform(planes.begin(), planes.end(), frustumPlanes.begin(), [](const auto& plane) {
        return glm::vec4{ plane.getNormal(), plane.getDistanceFromOrigin() };
    });
    mPostProcessingShader.setVec4Array(mFrustumPlanesLocation, frustumPlanes);

    glActiveTexture(GL_TEXTURE0 + mPositionTexture);
    glBindTexture(GL_TEXTURE_2D, Engine::renderSys.gBuffer.getPositionBuffer().getId());

    glActiveTexture(GL_TEXTURE0 + mNormalTexture);
    glBindTexture(GL_TEXTURE_2D, Engine::renderSys.gBuffer.getNormalBuffer().getId());

    glActiveTexture(GL_TEXTURE0 + mMaterialTexture);
    glBindTexture(GL_TEXTURE_2D, Engine::renderSys.gBuffer.getMaterialBuffer().getId());

    glActiveTexture(GL_TEXTURE0 + mBlurredSceneTexture);
    glBindTexture(GL_TEXTURE_2D, blurred.getId());
}

void SSR::setMaxDistance(float maxDistance) {
    if (mMaxDistance != maxDistance) {
        mMaxDistance = maxDistance;

        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setFloat("_ssr_rayMaxDistance", mMaxDistance);
    }
}

void SSR::setResolution(float resolution) {
    if (mResolution != resolution) {
        mResolution = resolution;

        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setFloat("_ssr_rayResolution", mResolution);
    }
}

void SSR::setSteps(std::int32_t steps) {
    if (mSteps != steps) {
        mSteps = steps;

        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setInt("_ssr_raySteps", mSteps);
    }
}

void SSR::setHitThreshold(float hitThreshold) {
    if (mHitThreshold != hitThreshold) {
        mHitThreshold = hitThreshold;

        ShaderScopedUsage useShader{ mPostProcessingShader };
        mPostProcessingShader.setFloat("_ssr_rayHitThreshold", mHitThreshold);
    }
}

SSR::~SSR() {
    Engine::renderSys.lightPassTarget.setRequireMipmap(false);

    Engine::renderSys.effectManager.releaseTexture(mPositionTexture);
    Engine::renderSys.effectManager.releaseTexture(mNormalTexture);
    Engine::renderSys.effectManager.releaseTexture(mMaterialTexture);
}
