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

        std::vector<GameObject> mGameObjects;

        HandleList<GameObject> mGameObjectsHL;

        glm::mat4 mProjection{1.0f};

        /** reference for the common matrix ubo */
        std::uint32_t mUboCommonMat;

        /** reference for the lights ubo */
        std::uint32_t mUboLights;

        std::vector<GameObjectEH> mLights;

        void initGL(std::uint32_t width, std::uint32_t height, float fovy, float nearPlane, float farPlane);

        void updateLights();

        void render();

        // private constructor, only the engine can create a render system
        RenderSystem();

    public:
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

        void createWindow(std::uint32_t width, std::uint32_t height, float fovy = glm::radians(45.0f), float nearPlane = 0.1f, float farPlane = 100.0f);

        /**
          * Creates a GameObject with a given Mesh and the corresponding Material
          * @param mesh the mesh used by this game object
          * @param material the material used to render the mesh
          * @return a handle to the created game object */
        GameObjectEH createGameObject(const Mesh& mesh, MaterialPtr material);

        /**
          * Creates an empty game object
          * @return a handle to the created game */
        GameObjectEH createGameObject();

        /**
          * Removes a game object and all its children.
          * All the external handles referencing these GameObject will become invalid.
          * @param go the game object to remove */
        void remove(const GameObjectEH& go);

        /**
          * Adds a light to the scene
          * if the GameObject does not have a Light component it is silently
          * discarded.
          * @param light a light component
          */
        void addLight(const GameObjectEH& light);

        void update();

        virtual ~RenderSystem();
};

#endif // RENDERSYSTEM_H
