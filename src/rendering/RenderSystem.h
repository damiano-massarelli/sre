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
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "RenderTarget.h"
#include "PointShadowMaterial.h"
#include "DeferredLightShader.h"
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
	/* PBR materials also store their data in the deferred renderer FBO.
	 * However, since this data is different from that stored by Blinn-Phong
	 * materials, two different shaders are used in the finalizeDeferredRendering.
	 * to use the right shader in the right fragment the pbr pass and normal-deferred pass
	 * store different values in the stencil buffer. The most significant bit is used to 
	 * distinguish them. The following 1 is needed because during stencilPass values are added
	 * or subtracted from this bits */
	static constexpr GLuint DEFERRED_STENCIL_MARK = 0x40; // 0100 0000
	static constexpr GLuint PBR_STENCIL_MARK      = 0xC0; // 1100 0000

	SDL_Window* mWindow = nullptr;

	glm::mat4 mProjection{ 1.0f };

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

	/** Shader used to render DirectionalLight's light on normal materials using deferred rendering */
	DeferredLightShader mDirectionalLightDeferred;

	/** Shader used to render DirectionalLight's light on PBR materials using deferred rendering */
	DeferredLightShader mDirectionalLightDeferredPBR;

	Shader mPointLightDeferredStencil; // shader used for stencil pass of point lights
	std::uint32_t mPointLightStencilLightIndexLocation = 0;
	std::uint32_t mPointLightStencilScaleLocation = 0;
	std::uint32_t mPointLightDeferredLightRadiusLocation = 0;

	/** Shader used to render PointLight's light on normal materials using deferred rendering */
	DeferredLightShader mPointLightDeferred; 

	/** Shader used to render PointLight's light on PBR materials using deferred rendering */
	DeferredLightShader mPointLightDeferredPBR;

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

	/** Updates the common matrices ubo */
	void updateMatrices(const glm::mat4* projection, const glm::mat4* view);

	/** Performs all the operations needed by all the rendering pipelines (deferred, pbr, ecc...) */
	void prepareRendering(const RenderTarget* target);

	/** Performs all operations needed by deferred rendering */
	void prepareDeferredRendering();

	/** Performs all operations needed by PBR rendering */
	void preparePBRRendering();

	/** Calls rendering submodules to perform rendering */
	void render(int phase);

	/** Performs all operations needed to finalize deferred rendering: combine g-buffer data */
	void finalizeDeferredRendering(const RenderTarget* target);

	void finalizeRendering();

	void stencilPass(int lightIndex, float radius);

	void pointLightPass(GLuint mark, DeferredLightShader& shaderWrapper);

	void directionalLightPass(GLuint mark, DeferredLightShader& shaderWrapper);

	/** Performs shadow mapping */
	void renderShadows();

	void renderDirectionalLightShadows(const DirectionalLight* light, const Transform& lightTransform);

	void renderPointLightShadows(const PointLight* light, const Transform& lightTransform);

	// private constructor, only the engine can create a render system
	RenderSystem();

	void cleanUp();

public:
	/** Creates a new window */
	void createWindow(std::uint32_t width, std::uint32_t height, float fovy = 0.785f, float nearPlane = 0.1, float farPlane = 1000.0f);

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

	/**
	 * @return the vertical fov
	 */
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
	 * @return the view matrix for the given Transform
	 */
	glm::mat4 getViewMatrix(const Transform& transform);

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