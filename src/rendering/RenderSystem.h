#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include "HandleList.h"
#include "GameObject.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "RenderPhase.h"
#include "EffectManager.h"
#include "FogSettings.h"
#include "ShadowMappingSettings.h"
#include "DeferredRenderingFBO.h"
#include "EffectsFBO.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "RenderTarget.h"
#include "PointShadowMaterial.h"
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <SDL.h>
#include <glm/gtx/quaternion.hpp>

/** The master renderer.
  * The master renderer manages all rendering settings
  * global for all renderers */
class RenderSystem
{
friend class Engine;

private:
    SDL_Window* mWindow = nullptr;

    glm::mat4 mProjection{1.0f};

	glm::mat4 mInvertView;

    /** reference for the common matrix ubo */
    std::uint32_t mUboCommonMat;

    /** reference for the lights ubo */
    std::uint32_t mUboLights;

    /** reference for camera ubo */
    std::uint32_t mUboCamera;

	/** Simple rect that represents the screen */
	Mesh mScreenMesh;
	/** Simple unit sphere used to render point lights in deferred rendering */
	Mesh mPointLightSphere;

	// used for rendering meshes for shadow mapping
	MaterialPtr mShadowMapMaterial;

	// used for rendering meshes with point lights
	std::shared_ptr<PointShadowMaterial> mPointShadowMaterial;

	/** Shaders used to render with deferred rendering */
	Shader mDirectionalLightDeferred; // shader used to do directional lighting
	std::uint32_t mDirectionalLightDeferredLightIndexLocation = 0;

	Shader mPointLightDeferredStencil; // shader used for stencil pass of point lights
	std::uint32_t mPointLightStencilLightIndexLocation = 0;
	std::uint32_t mPointLightStencilScaleLocation = 0;
	std::uint32_t mPointLightDeferredLightRadiusLocation = 0;

	Shader mPointLightDeferred; // shader used to do point light
	std::uint32_t mPointLightDeferredLightIndexLocation = 0;

	/** near and far clipping planes */
	float mNearPlane = 0.0f;
	float mFarPlane = 0.0f;
	float mVerticalFov = 0.0f;

    std::vector<GameObjectEH> mLights;

	/** Current rendering phase */
	int mRenderPhase;

    void initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane);

	void initDeferredRendering();

    /** Updates the lights ubo */
    void updateLights();

    /** Updates the camera ubo */
    void updateCamera();

	/** Updates the common matrices ubo (if toLighSpace is nullptr it is not updated) */
	void updateMatrices(const glm::mat4* projection, const glm::mat4* view, const glm::mat4* toLightSpace);

    /** Performs all operations needed by rendering */
    void prepareDeferredRendering(const RenderTarget* target);

	/** Calls rendering submodules to perform rendering */
	void render(int phase);

    /** Performs all operations needed to finalize deferred rendering: combine g-buffer data */
    void finalizeDeferredRendering(const RenderTarget* target);

	void finalizeRendering();

	void stencilPass(int lightIndex, float radius);

	void pointLightPass();

	void directionalLightPass();

	/** Performs shadow mapping */
	void renderShadows();

	void renderDirectionalLightShadows(const DirectionalLight* light, const Transform& lightTransform);

	void renderPointLightShadows(const PointLight* light, const Transform& lightTransform);

    // private constructor, only the engine can create a render system
    RenderSystem();

	void cleanUp();

public:
    /** Creates a new window */
    void createWindow(std::uint32_t width, std::uint32_t height, float fovy = 0.785f, float nearPlane = 0.1, float farPlane = 400.0f);

    /** Maximum number of lights */
    static constexpr std::size_t MAX_LIGHT_NUMBER = 32;

    /** The index of the common matrices uniform block */
    static constexpr std::uint32_t COMMON_MAT_UNIFORM_BLOCK_INDEX = 0;

    /** The index of uniform block used for lights */
    static constexpr std::uint32_t LIGHT_UNIFORM_BLOCK_INDEX = 1;

    /** The index of uniform block used for lights */
    static constexpr std::uint32_t CAMERA_UNIFORM_BLOCK_INDEX = 2;

	static constexpr std::uint32_t FOG_UNIFORM_BLOCK_INDEX = 3;

	static constexpr std::uint32_t SHADOWMAP_UNIFORM_BLOCK_INDEX = 4;

	/** fbo used for deferred rendering */
	DeferredRenderingFBO deferredRenderingFBO;

	/** fbo used to render effects */
	RenderTarget effectTarget;

	/** settings for shadow mapping */
	ShadowMappingSettings shadowMappingSettings;

	/** settings for fog */
	FogSettings fogSettings;

    /** The camera used for rendering */
    GameObjectEH camera;

	/** The effect manager handles post processing effects */
	EffectManager effectManager;

public:
    // Cannot copy this system, only the engine has an instance
    RenderSystem(const RenderSystem& rs) = delete;
    RenderSystem& operator=(const RenderSystem& rs) = delete;

    /**
     * Adds a light to the scene
     * if the GameObject does not have a Light component it is silently
     * discarded.
     * @param light a GameObjectEH. The referenced GameObject should contain a Light component.
     */

    void addLight(const GameObjectEH& light);

	/**
	 * @return the width of the current window
	 */
	std::int32_t getScreenWidth() const;

	/**
	 * @return the height of the current window
	 */
	std::int32_t getScreenHeight() const;

	/**
	 * @return the near clipping plane distance
	 */
	float getNearPlane() const;

	/**
	 * @return the far clipping plane distance
	 */
	float getFarPlane() const;

	float getVerticalFov() const;

	/**
	 * @return the current rendering phase
	 * @see RenderPhase
	 */
	int getRenderPhase() const;

	/** 
	 * Render the current scene.
	 * @param renderTarget the target onto which the scene is rendered. if nullptr the screen is used.
	 * @param phase specifies which render phase to use
	 */
	void renderScene(const RenderTarget* target = nullptr, RenderPhase phase = RenderPhase::NONE);

	/**
	 * Returns the view matrix for a certain transform.
	 * The actual matrix is multiplied by mInvertView
	 */
	glm::mat4 getViewMatrix(const Transform& transform);

	void enableClipPlane() const;

	void disableClipPlane() const;

	void setClipPlane(const glm::vec4& clipPlane) const;

	/**
	 * Copies a texture into another applying a shader.
	 * @param src the source texture
	 * @param dst a render target whose color buffer contains the destination texture.
	 * @param shader the shader used to copy the texture
	 * @param clear clear the dst texture before copying. Set it to false if src and destination texture are the same
	 */
	void copyTexture(const Texture& src, RenderTarget& dst, const Shader& shader, bool clear = true);

	virtual ~RenderSystem() = default;
};

#endif // RENDERSYSTEM_H
