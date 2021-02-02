#include "SSR.h"
#include "Engine.h"
#include "rendering/RenderSystem.h"
#include "rendering/effects/EffectManager.h"
#include <cameras/CameraComponent.h>
#include <cassert>
#include <vector>

SSR::SSR()
    : Effect{ "ssr", "effects/ssr.glsl" } {

    ShaderScopedUsage useShader{ mPostProcessingShader };
    mPostProcessingShader.setInt("inputTexture", 0);
    mPostProcessingShader.setInt("reflectionTexture", 1);

    // Create shader to extract reflections
    createAndSetupExtractShader();

    auto settings = GBuffer::DIFFUSE_BUFFER_SETTINGS;
    settings.internalFormat = GL_RGBA16F;
    settings.appearanceOptions.hasMipmap = true;
    mSSROutput
        = Texture::load(nullptr, Engine::renderSys.getScreenWidth(), Engine::renderSys.getScreenHeight(), settings);

    for (int i = 0; i < 6; ++i) {
        mSSROutpuRenderTargets.emplace_back(&mSSROutput, nullptr, i, 0);
    }

    // Start at 1, the first mipmap level contains the non-blurred ssr output
    for (int i = 1; i < 6; ++i) {
        mGaussianBlurEffects.emplace_back(&(mSSROutpuRenderTargets[i]));
    }
}

void SSR::createAndSetupExtractShader() {
    mSSRExtract = Shader::loadFromFile(
        std::vector<std::string>{ "effects/genericEffectVS.glsl" }, {}, { "effects/ssrCreateFS.glsl" });

    ShaderScopedUsage useShader{ mSSRExtract };
    // Textures
    mSSRExtract.setInt("_ssr_sceneTexture", 0);
    mSSRExtract.setInt("_ssr_depthBuffer", 1);
    mSSRExtract.setInt("_ssr_position", 2);
    mSSRExtract.setInt("_ssr_normals", 3);
    mSSRExtract.setInt("_ssr_materialBuffer", 4);
    mSSRExtract.setInt("_ssr_diffuseTexture", 5);
    mSSRExtract.setInt("_ssr_fallbackSkybox", 6);

    // Params
    mSSRExtract.setFloat("_ssr_rayMaxDistance", mMaxReflectionDistance);
    mSSRExtract.setInt("_ssr_numSamples", mNumSamples);
    mSSRExtract.setInt("_ssr_raySteps", mSteps);
    mSSRExtract.setFloat("_ssr_rayHitThreshold", mHitThreshold);
    mSSRExtract.setFloat("_ssr_steepAngleHitThresholdMultiplier", mSteepAngleHitThresholdMultiplier);

    // Cached shader locations
    mCameraPositionLocation = mSSRExtract.getLocationOf("_ssr_cameraPosition");
    mProjectionViewLocation = mSSRExtract.getLocationOf("_ssr_projectionView");
    mFrustumPlanesLocation = mSSRExtract.getLocationOf("_ssr_frustumPlanes");
}

void SSR::update() {
    RenderSystem& renderSystem = Engine::renderSys;

    {
        ShaderScopedUsage useShader{ mSSRExtract };

        // Setup shader
        const glm::mat4 currentProjectViewMatrix
            = renderSystem.getProjectionMatrix() * renderSystem.getViewMatrix(renderSystem.getCamera()->transform);
        const auto cameraComponent = renderSystem.getCamera()->getComponent<CameraComponent>();
        const float near = cameraComponent->getNearPlaneDistance();
        const float far = cameraComponent->getFarPlaneDistance();

        if (near != mNearPlane) {
            mNearPlane = near;
            mSSRExtract.setFloat("_ssr_nearPlane", mNearPlane);
        }
        if (far != mFarPlane) {
            mFarPlane = far;
            mSSRExtract.setFloat("_ssr_farPlane", mFarPlane);
        }

        mSSRExtract.setVec3(mCameraPositionLocation, renderSystem.getCamera()->transform.getPosition());
        mSSRExtract.setMat4(mProjectionViewLocation, currentProjectViewMatrix);

        // Set camera frustum planes
        std::vector<glm::vec4> frustumPlanes{ 6 };
        std::array<Plane, 6> planes
            = Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getViewFrutsum().getPlanes();
        std::transform(planes.begin(), planes.end(), frustumPlanes.begin(), [](const auto& plane) {
            return glm::vec4{ plane.getNormal(), plane.getDistanceFromOrigin() };
        });
        mSSRExtract.setVec4Array(mFrustumPlanesLocation, frustumPlanes);
    }
}

void SSR::applyEffect(const Texture& input, const RenderTarget* dst) {
    RenderSystem& renderSystem = Engine::renderSys;

    // Extract SSR into mSSROutputRenderTargets[0]
    renderSystem.copyTexture({ renderSystem.lightPassTarget,
                                 renderSystem.gBuffer.getDepthBuffer(),
                                 renderSystem.gBuffer.getPositionBuffer(),
                                 renderSystem.gBuffer.getNormalBuffer(),
                                 renderSystem.gBuffer.getMaterialBuffer(),
                                 renderSystem.gBuffer.getDiffuseBuffer(),
                                 mFallbackSkybox },
        mSSROutpuRenderTargets[0],
        mSSRExtract,
        false);

    // Blur output for roughness
    for (std::size_t i = 0; i < mGaussianBlurEffects.size(); ++i) {
        mGaussianBlurEffects[i].getBlurred(mSSROutput, 1);
    }

    Effect::applyEffect({ input, mSSROutput }, dst);
}

void SSR::setMaxReflectionDistance(float maxReflectionDistance) {
    if (mMaxReflectionDistance != maxReflectionDistance) {
        mMaxReflectionDistance = maxReflectionDistance;

        ShaderScopedUsage useShader{ mSSRExtract };
        mSSRExtract.setFloat("_ssr_rayMaxDistance", mMaxReflectionDistance);
    }
}

void SSR::setNumSamples(std::int32_t numSamples) {
    if (mNumSamples != numSamples) {
        mNumSamples = numSamples;

        ShaderScopedUsage useShader{ mSSRExtract };
        mSSRExtract.setInt("_ssr_numSamples", numSamples);
    }
}

void SSR::setSteps(std::int32_t steps) {
    if (mSteps != steps) {
        mSteps = steps;

        ShaderScopedUsage useShader{ mSSRExtract };
        mSSRExtract.setInt("_ssr_raySteps", mSteps);
    }
}

void SSR::setHitThreshold(float hitThreshold) {
    if (mHitThreshold != hitThreshold) {
        mHitThreshold = hitThreshold;

        ShaderScopedUsage useShader{ mSSRExtract };
        mSSRExtract.setFloat("_ssr_rayHitThreshold", mHitThreshold);
    }
}

void SSR::setSteepAngleHitThresholdMultiplier(float multiplier) {
    if (mSteepAngleHitThresholdMultiplier != multiplier) {
        mSteepAngleHitThresholdMultiplier = multiplier;

        ShaderScopedUsage useShader{ mSSRExtract };
        mSSRExtract.setFloat("_ssr_steepAngleHitThresholdMultiplier", mSteepAngleHitThresholdMultiplier);
    }
}

void SSR::setFallbackSkyboxTexture(Texture fallbackSkybox) {
#ifdef SRE_DEBUG
    assert(!fallbackSkybox.isValid() || fallbackSkybox.isCubeMap());
#endif  // SRE_DEBUG
    ShaderScopedUsage useShader{ mSSRExtract };
    mSSRExtract.setInt("_ssr_useFallbackSkybox", fallbackSkybox.isValid() && fallbackSkybox.isCubeMap());
    mFallbackSkybox = fallbackSkybox;
}
