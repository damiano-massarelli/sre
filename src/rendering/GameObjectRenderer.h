#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H
#include "GameObject.h"

class GameObjectRenderer
{
    friend class Engine;

    private:
        /** Actually renders a Mesh with its Material */
        void draw(Mesh mesh, MaterialPtr material);

        GameObjectRenderer() = default;

    public:
        GameObjectRenderer(const GameObjectRenderer& gor) = delete;

        GameObjectRenderer& operator=(const GameObjectRenderer& gor) = delete;

        /**
          * Renders a list of GameObject%s.
          * @param gameObjects the GameObject%s to render */
        void render(const std::vector<GameObject>& gameObjects);

        virtual ~GameObjectRenderer() = default;
};

#endif // GAMEOBJECTRENDERER_H
