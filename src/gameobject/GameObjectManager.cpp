#include "GameObjectManager.h"
#include "Engine.h"

GameObjectManager::GameObjectManager() : mGameObjectsHL{mGameObjects}
{

}

GameObjectEH GameObjectManager::createGameObject(const Mesh& mesh, MaterialPtr material)
{
    std::uint32_t index, gen;
    mGameObjectsHL.add(GameObject(mesh, material), index, gen);
    return GameObjectEH(&mGameObjectsHL, index, gen);
}

GameObjectEH GameObjectManager::createGameObject()
{
    std::uint32_t index, gen;
    mGameObjectsHL.add(GameObject(), index, gen);
    return GameObjectEH(&mGameObjectsHL, index, gen);
}

void GameObjectManager::remove(const GameObjectEH& go)
{
    /* GameObjects form a hierarchy through their transform objects. Removing GameObjects recursively
     * (a parent calls remove on its children) is, however, impossible. In fact, while we are operating on
     * a GameObject living in a vector (mGameObjects) we also change this vector by deleting its children. When
     * control goes back to the parent GameObject the vector it is living in might be moved somewhere else in memory
     * thus invalidating pointers (among which the this pointer). The solution here is to batch delete a node and
     * all its children. Another solution might be to delete the parent first and then its children */

    go->transform.removeParent(); // breaks the hierarchy here
    std::vector<GameObjectEH> toRemove{go};

    // fills this vector with all the GameObjects in the hierarchy
    for (std::size_t i = 0; i < toRemove.size(); ++i) {
        const auto& children = (toRemove[i])->transform.getChildren();
        toRemove.insert(toRemove.end(), children.begin(), children.end());
    }

    // cleans up and removes all the GameObjects in the hierarchy
    for (auto& rem : toRemove) {
        rem->cleanUp();
        mGameObjectsHL.remove(rem.mHandleIndex, rem.mGeneration);
    }
}

void GameObjectManager::update()
{
    Engine::gameObjectRenderer.render(mGameObjects);
}

GameObjectManager::~GameObjectManager()
{
    // no need to remove them, just clean up what they
    // allocated.
    for (auto& go : mGameObjects)
        go.cleanUp();
}

