#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <SDL.h>
#include <glm/gtx/quaternion.hpp>
#include "HandleList.h"
#include "GameObject.h"

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

        std::vector<GameObjectEH> mLights;

        void initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane);

        void updateLights();

        void draw(Mesh mesh, MaterialPtr material);

        // private constructor, only the engine can create a render system
        RenderSystem();

    public:
        /** Creates a new window */
        void createWindow(std::uint32_t width, std::uint32_t height, float fovy = 0.785f, float nearPlane = 0.1, float farPlane = 200.0f);

        /** Maximum number of lights */
        static constexpr std::size_t MAX_LIGHT_NUMBER = 10;

        /** The index of the common matrices uniform block */
        static constexpr std::uint32_t COMMON_MAT_UNIFORM_BLOCK_INDEX = 0;

        /** The index of uniform block used for lights */
        static constexpr std::uint32_t LIGHT_UNIFORM_BLOCK_INDEX = 1;

        GameObjectEH camera;

    public:
        // Cannot copy this system, only the engine has an instance
        RenderSystem(const RenderSystem& rs) = delete;
        RenderSystem& operator=(const RenderSystem& rs) = delete;

        /**
          * Adds a light to the scene
          * if the GameObject does not have a Light component it is silently
          * discarded.
          * @param light a light component
          */
        void addLight(const GameObjectEH& light);

        void render(const std::vector<GameObject>& gameObjects);

        virtual ~RenderSystem();
};

#endif // RENDERSYSTEM_H
