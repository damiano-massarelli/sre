#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H
#include "gameobject/GameObject.h"
#include "components/HandleList.h"
#include "gameobject/GameObjectEH.h"

class GameObjectManager
{
    friend class Engine;
	friend class GameObjectRenderer;

    private:
        GameObjectManager();

        std::vector<GameObject> mGameObjects;

        HandleList<GameObject> mGameObjectsHL;

		void cleanUp();

    public:
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
		 * @return all the GameObject%s.
		 */
		const std::vector<GameObject>& getGameObjects() const;

		virtual ~GameObjectManager() = default;
};

#endif // GAMEOBJECTMANAGER_H
