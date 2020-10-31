#include "rendering/materials/WaterMaterial.h"
#include "Engine.h"
#include "cameras/CameraComponent.h"
#include "rendering/RenderPhase.h"
#include "rendering/RenderSystem.h"
#include <cmath>
#include <glm/glm.hpp>

WaterMaterial::WaterMaterial(float waterY, const Texture& dudv, const Texture& normalMap)
    : Material{ "shaders/waterVS.glsl", "shaders/waterFS.glsl" }
    , mWaterY{ waterY }
    , mDuDvMap{ dudv }
    , mNormalMap{ normalMap } {
    // mEmptyReflectionTarget.create(320, 180, false, false);
    // mReflectionFbo.init(320, 180);

    // mEmptyRefractionTarget.create(1280, 720, false, false);
    // mRefractionFbo.init(1280, 720);

    ///**
    // * This is a target whose initial color is the diffuse color added by the
    // * deferred and pbr rendering. On top of that, the result of forward
    // * rendering and particle rendering is added in another render pass (see
    // * renderRefraction)
    // */
    // mReflectionRarget.createWith(mReflectionFbo.getDiffuseBuffer(), mReflectionFbo.getDepthBuffer());

    // mReflectionCamera = Engine::gameObjectManager.createGameObject();

    //// don't render when rendering for water or shadows
    // unSupportedRenderPhases |= RenderPhase::ALL & ~RenderPhase::DEFERRED_RENDERING;

    // mEventCrumb = Engine::eventManager.addListenerFor(EventManager::PRE_RENDER_EVENT, this, true);

    // ShaderScopedUsage useShader{ shader };

    // shader.setInt("reflection", 0);
    // shader.setInt("refraction", 1);
    // shader.setInt("dudvMap", 2);
    // shader.setInt("normalMap", 3);
    // shader.setInt("depthMap", 4);

    // shader.setFloat("near", Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getNearPlaneDistance());
    // shader.setFloat("far", Engine::renderSys.getCamera()->getComponent<CameraComponent>()->getFarPlaneDistance());

    // shader.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);
    // shader.bindUniformBlock("Camera", RenderSystem::CAMERA_UNIFORM_BLOCK_INDEX);

    // mMoveDuDvLocation = shader.getLocationOf("moveDuDv");
}

void WaterMaterial::onEvent(SDL_Event e) {
    float delta = (*(static_cast<float*>(e.user.data1)));
    mMoveDuDv += waveSpeed * delta / 1000.0f;
    mMoveDuDv = std::fmod(mMoveDuDv, 1.0f);

    auto& renderSys = Engine::renderSys;

    /*
     * Do not render shadows now. Just use shadow maps from the last frame,
     * it should be good enough! What's more, shadows would not make sense since
     * the scene is split due to clipping planes.
     */
    bool enabled = renderSys.shadowMappingSettings.isShadowRenderingEnabled();
    if (enabled)
        renderSys.shadowMappingSettings.disableShadowRendering();

    // auto oldDeferredFbo = renderSys.gBuffer;
    renderSys.enableClipPlane();

    renderRefraction();

    renderReflection();

    if (enabled)
        renderSys.shadowMappingSettings.enableShadowRendering();

    renderSys.disableClipPlane();
    // renderSys.gBuffer = oldDeferredFbo;
}

std::size_t WaterMaterial::hash() const { return Material::hash() + mDuDvMap.getId() + mNormalMap.getId(); }

bool WaterMaterial::equalsTo(const Material* rhs) const {
    if (shader.getId() != rhs->shader.getId())
        return false;

    auto other = static_cast<const WaterMaterial*>(rhs);

    return Material::equalsTo(rhs) && mDuDvMap.getId() == other->mDuDvMap.getId()
        && mNormalMap.getId() == other->mNormalMap.getId() && waveSpeed == other->waveSpeed
        && mWaterY == other->mWaterY;
}

void WaterMaterial::renderReflection() {
    GameObjectEH oldCamera = Engine::renderSys.getCamera();
    mReflectionCamera->transform = oldCamera->transform;

    // temp camera for this stage
    Engine::renderSys.setCamera(mReflectionCamera);

    Transform& camTransform = mReflectionCamera->transform;

    // invert the pitch of the camera
    auto camUp = camTransform.up();
    auto up = glm::vec3{ 0, 1, 0 };
    auto cosUp = glm::dot(up, camUp);
    auto angle = glm::acos(cosUp);
    float sign = glm::sign(glm::dot(glm::cross(camUp, up), camTransform.right()));
    camTransform.rotateBy(glm::normalize(glm::angleAxis(sign * 2 * angle, camTransform.right())));

    // put camera under water
    float offset = camTransform.getPosition().y - mWaterY;
    camTransform.moveBy(glm::vec3{ 0, -2 * offset, 0 });

    // render to reflection target
    // Engine::renderSys.gBuffer = mReflectionFbo;
    Engine::renderSys.setClipPlane(glm::vec4{ 0, 1, 0, -mWaterY });
    Engine::renderSys.renderScene(&mEmptyReflectionTarget, RenderPhase::WATER);

    glBindFramebuffer(GL_FRAMEBUFFER, mReflectionRarget.getFbo());
    Engine::renderSys.render(RenderPhase::FORWARD_RENDERING | RenderPhase::WATER);

    Engine::particleRenderer.render();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render particles
    Engine::particleRenderer.render();

    Engine::renderSys.setCamera(oldCamera);
}

void WaterMaterial::renderRefraction() {
    // Engine::renderSys.gBuffer = mRefractionFbo;
    Engine::renderSys.setClipPlane(glm::vec4{ 0, -1, 0, mWaterY + 1 });
    Engine::renderSys.renderScene(&mEmptyRefractionTarget, RenderPhase::WATER);
}

void WaterMaterial::use() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mReflectionRarget.getColorBuffer()->getId());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mRefractionFbo.getDiffuseBuffer().getId());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mDuDvMap.getId());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mNormalMap.getId());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mRefractionFbo.getDepthBuffer().getId());

    shader.use();
    shader.setFloat(mMoveDuDvLocation, mMoveDuDv);
}

void WaterMaterial::after() {
    for (int i = 0; i <= 7; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    shader.stop();
}