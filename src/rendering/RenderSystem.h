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
#include "ShadowMappingSettings.h"
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

		/** reference to screen frame buffer */
		std::uint32_t mScreenFbo;
		Texture mColorBuffer;
		Texture mDepthBuffer;
		Mesh mScreenMesh;

		/** reference to shadow map frame buffer */
		std::uint32_t mShadowFbo;
		Texture mShadowMap;
		// used for rendering meshes for shadow mapping
		MaterialPtr mShadowMapMaterial;

		/** near and far clipping planes */
		float mNearPlane = 0.0f;
		float mFarPlane = 0.0f;
		float mVerticalFov = 0.0f;

        std::vector<GameObjectEH> mLights;

		/** Current rendering phase */
		RenderPhase mRenderPhase;

        void initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane);

		void initScreenFbo();

		void initShadowFbo();

        /** Updates the lights ubo */
        void updateLights();

        /** Updates the camera ubo */
        void updateCamera();

		

        /** Performs all operations needed by rendering */
        void prepareRendering();

		/** Calls rendering submodules to perform rendering */
		void render(RenderPhase phase = RenderPhase::NORMAL);

        /** Performs all operations needed to finalize rendering: blitting to screen */
        void finalizeRendering();

		/** Performs shadow mapping */
		void renderShadows();

        // private constructor, only the engine can create a render system
        RenderSystem();

		void cleanUp();

    public:
        /** Creates a new window */
        void createWindow(std::uint32_t width, std::uint32_t height, float fovy = 0.785f, float nearPlane = 0.1, float farPlane = 200.0f);

        /** Maximum number of lights */
        static constexpr std::size_t MAX_LIGHT_NUMBER = 10;

        /** The index of the common matrices uniform block */
        static constexpr std::uint32_t COMMON_MAT_UNIFORM_BLOCK_INDEX = 0;

        /** The index of uniform block used for lights */
        static constexpr std::uint32_t LIGHT_UNIFORM_BLOCK_INDEX = 1;

        /** The index of uniform block used for lights */
        static constexpr std::uint32_t CAMERA_UNIFORM_BLOCK_INDEX = 2;

		ShadowMappingSettings shadowMappingSettings;

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
		 * @return the current rendering pahse
		 * @sa RenderPhase
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
