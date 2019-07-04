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

	/** reference to shadow map frame buffer */
	std::uint32_t mShadowFbo;
	Texture mShadowMap;
	// used for rendering meshes for shadow mapping
	MaterialPtr mShadowMapMaterial;

	/** Shaders used to render with deferred rendering */
	Shader mDirectionalLightDeferred; // shader used to do directional lighting
	std::uint32_t mDirectionalLightDeferredLightIndexLocation = 0;

	Shader mPointLightDeferredStencil; // shader used for stencil pass of point lights
	std::uint32_t mPointLightStencilLightIndexLocation = 0;
	std::uint32_t mPointLightStencilScaleLocation = 0;

	Shader mPointLightDeferred; // shader used to do point light
	std::uint32_t mPointLightDeferredLightIndexLocation = 0;


	/** fbo used to render effects */
	EffectsFBO mEffectFBO;

	/** near and far clipping planes */
	float mNearPlane = 0.0f;
	float mFarPlane = 0.0f;
	float mVerticalFov = 0.0f;

    std::vector<GameObjectEH> mLights;

	/** Current rendering phase */
	RenderPhase mRenderPhase;

    void initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane);

	void initDeferredRendering();

	void initShadowFbo();

    /** Updates the lights ubo */
    void updateLights();

    /** Updates the camera ubo */
    void updateCamera();

    /** Performs all operations needed by rendering */
    void prepareDeferredRendering();

	/** Performs deferred and forward rendering */
	void renderScene();

	/** Calls rendering submodules to perform rendering */
	void render(RenderPhase phase);

    /** Performs all operations needed to finalize deferred rendering: combine g-buffer data */
    void finalizeDeferredRendering();

	void finalizeRendering();

	void stencilPass(int lightIndex, float radius);

	void pointLightPass();

	void directionalLightPass();

	/** Performs shadow mapping */
	void renderShadows();

	float computePointLightRadius(const std::shared_ptr<Light>& light) const;

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
	RenderPhase getRenderPhase() const;

	/**
		* Returns the view matrix for a certain transform.
		* The actual matrix is multiplied by mInvertView
		*/
	glm::mat4 getViewMatrix(const Transform& transform);

	virtual ~RenderSystem() = default;
};

#endif // RENDERSYSTEM_H
