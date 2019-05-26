#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <SDL.h>
#include <glm/gtx/quaternion.hpp>
#include "HandleList.h"
#include "GameObject.h"

/** The master renderer.
  * The master renderer manages all rendering settings
  * global for all renderers */
class RenderSystem
{
    friend class Engine;

    private:
        SDL_Window* mWindow = nullptr;

        glm::mat4 mProjection{1.0f};

        /** reference for the common matrix ubo */
        std::uint32_t mUboCommonMat;

        /** reference for the lights ubo */
        std::uint32_t mUboLights;

        /** reference for camera ubo */
        std::uint32_t mUboCamera;

        std::vector<GameObjectEH> mLights;

        void initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane);

        /** Updates the lights ubo */
        void updateLights();

        /** Updates the camera ubo */
        void updateCamera();

        /** Performs all operations needed by rendering */
        void prepareRendering();

        /** Performs all operations needed to finalize rendering: blitting to screen */
        void finalizeRendering();

        // private constructor, only the engine can create a render system
        RenderSystem();

    public:
        /** Creates a new window */
        void createWindow(std::uint32_t width, std::uint32_t height, float fovy = 0.785f, float nearPlane = 0.1, float farPlane = 400.0f, std::uint32_t samples = 4);

        /** Maximum number of lights */
        static constexpr std::size_t MAX_LIGHT_NUMBER = 10;

        /** The index of the common matrices uniform block */
        static constexpr std::uint32_t COMMON_MAT_UNIFORM_BLOCK_INDEX = 0;

        /** The index of uniform block used for lights */
        static constexpr std::uint32_t LIGHT_UNIFORM_BLOCK_INDEX = 1;

        /** The index of uniform block used for lights */
        static constexpr std::uint32_t CAMERA_UNIFORM_BLOCK_INDEX = 2;

        /** The camera used for rendering */
        GameObjectEH camera;

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

        virtual ~RenderSystem();
};

#endif // RENDERSYSTEM_H
