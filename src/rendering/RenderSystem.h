#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include "components/HandleList.h"
#include "gameobject/GameObject.h"
#include "rendering/RenderPhase.h"
#include "rendering/RenderTarget.h"
#include "rendering/deferredRendering/DeferredLightShader.h"
#include "rendering/deferredRendering/DeferredRenderingFBO.h"
#include "rendering/effects/EffectManager.h"
#include "rendering/fog/FogSettings.h"
#include "rendering/light/DirectionalLight.h"
#include "rendering/light/Light.h"
#include "rendering/light/PointLight.h"
#include "rendering/materials/Material.h"
#include "rendering/materials/PointShadowMaterial.h"
#include "rendering/materials/Shader.h"
#include "rendering/materials/Texture.h"
#include "rendering/mesh/Mesh.h"
#include "rendering/shadow/ShadowMappingSettings.h"
#include <SDL.h>
#include <cstdint>
#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <vector>

/** The master renderer.
 * The master renderer manages all rendering settings
 * global for all renderers */
class RenderSystem {
    friend class Engine;

private:
    SDL_Window* mWindow = nullptr;

    glm::mat4 mProjection{ 1.0f };

    glm::mat4 mInvertView{ 0.f };

    /** reference for the common matrix ubo */
    GLuint mUboCommonMat = 0;

    /** reference for the lights ubo */
    GLuint mUboLights = 0;

    /** reference for camera ubo */
    GLuint mUboCamera = 0;

    /** Simple rect that represents the screen */
    Mesh mScreenMesh;

    /** Simple unit sphere used to render point lights in deferred rendering */
    Mesh mPointLightSphere;

    // used for rendering meshes for shadow mapping
    MaterialPtr mShadowMapMaterial;

    // used for rendering meshes with point lights
    std::shared_ptr<PointShadowMaterial> mPointShadowMaterial;

    /** Shader used to render DirectionalLight's light on normal materials using deferred rendering */
    DeferredLightShader mDirectionalLightDeferred;

    /** Shader used to render DirectionalLight's light on PBR materials using deferred rendering */
    DeferredLightShader mDirectionalLightDeferredPBR;

    Shader mPointLightDeferredStencil;  // shader used for stencil pass of point lights
    std::uint32_t mPointLightStencilLightIndexLocation = 0;
    std::uint32_t mPointLightStencilScaleLocation = 0;
    std::uint32_t mPointLightDeferredLightRadiusLocation = 0;

    /** Shader used to render PointLight's light on normal materials using deferred rendering */
    DeferredLightShader mPointLightDeferred;

    /** Shader used to render PointLight's light on PBR materials using deferred rendering */
    DeferredLightShader mPointLightDeferredPBR;

    /** The camera used for rendering */
    GameObjectEH mCamera;

    std::vector<GameObjectEH> mLights;

    /** Current rendering phase */
    int mRenderPhase;

    void initGL(std::uint32_t width, std::uint32_t height);

    void initDeferredRendering();

    /** Updates the lights ubo */
    void updateLights();

    /** Updates the camera ubo */
    void updateCamera();

    /** Updates the common matrices ubo */
    void updateMatrices(const glm::mat4* projection, const glm::mat4* view);

    /** Performs all the operations needed by all the rendering pipelines (deferred, pbr, ecc...) */
    void prepareRendering(const RenderTarget* target);

    /** Performs all operations needed by deferred rendering */
    void prepareDeferredRendering();

    /** Performs all operations needed to finalize deferred rendering: combine g-buffer data */
    void finalizeDeferredRendering(const RenderTarget* target);

    void finalizeRendering();

    void stencilPass(int lightIndex, float radius, bool renderingToScreen);

    void pointLightPass(DeferredLightShader& shaderWrapper, bool renderingToScreen);

    void directionalLightPass(DeferredLightShader& shaderWrapper);

    /** Performs shadow mapping */
    void renderShadows();

    void renderDirectionalLightShadows(const DirectionalLight* light, const Transform& lightTransform);

    void renderPointLightShadows(const PointLight* light, const Transform& lightTransform);

    // private constructor, only the engine can create a render system
    RenderSystem() = default;

    void setDefaultCamera();

    void cleanUp();

    void shutdown();

public:
    /** Creates a new window */
    void createWindow(std::uint32_t width, std::uint32_t height);

    SDL_Window* getWindow() const { return mWindow; }

    /** Maximum number of lights */
    static constexpr std::size_t MAX_LIGHT_NUMBER = 256;

    /** The index of the common matrices uniform block */
    static constexpr std::uint32_t COMMON_MAT_UNIFORM_BLOCK_INDEX = 0;

    /** The index of uniform block used for lights */
    static constexpr std::uint32_t LIGHT_UNIFORM_BLOCK_INDEX = 1;

    /** The index of uniform block used for lights */
    static constexpr std::uint32_t CAMERA_UNIFORM_BLOCK_INDEX = 2;

    static constexpr std::uint32_t FOG_UNIFORM_BLOCK_INDEX = 3;

    static constexpr std::uint32_t SHADOWMAP_UNIFORM_BLOCK_INDEX = 4;

    /** fbo used for deferred rendering */
    GBuffer gBuffer;

    /** The texture that contains the outcome of the light pass */
    Texture lightPassTarget;
    /** RenderTarget used as target for the render pass */
    RenderTarget lightPassRenderTarget;

    /** settings for shadow mapping */
    ShadowMappingSettings shadowMappingSettings;

    /** settings for fog */
    FogSettings fogSettings;

    /** The effect manager handles post processing effects */
    EffectManager effectManager;

    // Cannot copy this system, only the engine has an instance
    RenderSystem(const RenderSystem& rs) = delete;
    RenderSystem& operator=(const RenderSystem& rs) = delete;

    /**
     * Adds a light to the scene.
     * if the GameObject does not have a Light component it is silently
     * discarded.
     * @param light a GameObjectEH. The referenced GameObject should contain a Light component.
     */
    void addLight(const GameObjectEH& light);

    /**
     * Removes a light from the scene.
     * @param light a GameObjectEH. The referenced GameObject should have been
     * previously added to the scene using addLight.
     */
    void removeLight(const GameObjectEH& light);

    /**
     * @return the width of the current window
     */
    std::int32_t getScreenWidth() const;

    /**
     * Sets the camera used for rendering.
     * @param camera the camera to use (it must contain a CameraComponent)
     */
    void setCamera(const GameObjectEH& camera);

    /**
     * @returns the current camera.
     */
    GameObjectEH getCamera() const;

    /**
     * @return the height of the current window
     */
    std::int32_t getScreenHeight() const;

    /**
     * @return the current rendering phase
     * @see RenderPhase
     */
    int getRenderPhase() const;

    /**
     * Render the current scene.
     * @param renderTarget the target onto which the scene is rendered. if
     * nullptr the screen is used. If the depth buffer of the rendered scene is not needed, it is advisable
     * to construct the RenderTarget using the already existing gBuffer's depthBuffer. This can improve performances
     * and can be achieved using RenderTarget{<colorBuffer>, Engine::renderSys.gBuffer.getDepthBuffer()}.
     * @param phase specifies which render phase to use
     */
    void renderScene(const RenderTarget* target = nullptr, RenderPhase phase = RenderPhase::NONE);

    /**
     * Renders all the GameObject%s to the currently bound RenderTarget.
     * renderScene() should always be preferred except for particular cases
     * @param phase the rendering phase */
    void render(int phase);

    /**
     * Returns the view matrix for a certain transform.
     * The actual matrix is multiplied by mInvertView
     * @return the view matrix for the given Transform
     */
    glm::mat4 getViewMatrix(const Transform& transform) const;

    /**
     * @return the projection matrix
     */
    const glm::mat4 getProjectionMatrix() const;

    /**
     * Enables clip planes.
     * @see setClipPlane
     */
    void enableClipPlane() const;

    /**
     * Disables clip planes.
     * @see enableClipPlane
     */
    void disableClipPlane() const;

    /**
     * Sets the plane equation for the clip plane.
     * @param clipPlane the clip plane equation.
     */
    void setClipPlane(const glm::vec4& clipPlane) const;

    /**
     * Copies a Texture into another applying a Shader.
     * @param src the source texture.
     * @param dst a RenderTarget whose color buffer contains the destination Texture.
     * @param shader the shader used to copy the Texture.
     * @param clear whether the dst texture should be cleared before copying. Set it to false if src and destination texture are the same
     */
    void copyTexture(const Texture& src, const RenderTarget& dst, Shader& shader, bool clear = true);

    /**
     * Copies some Texture%s into another applying a Shader.
     * The given source Texture%s will be bound to the 0th to the (sources.size - 1)th texture slots.
     * The source Texture%s can be combined together using the provided Shader.
     * @param sources the input textures.
     * @param dst a RenderTarget whose color buffer contains the destination Texture.
     * @param shader the Shader used to copy the input Texture%s.
     * @param clear whether the dst texture should be cleared before copying.
     */
    void copyTexture(const std::vector<std::reference_wrapper<const Texture>>& sources, const RenderTarget& dst, Shader& shader, bool clear = true);

    virtual ~RenderSystem() = default;
};

#endif  // RENDERSYSTEM_H
