#include "gameobject/GameObjectEH.h"
#include "gameobject/GameObject.h"

GameObjectEH::GameObjectEH(HandleList<GameObject>* handleList, std::uint32_t index, std::uint32_t generation)
    : ExternalHandle{ index, generation }
    , mHandleList{ handleList } {
    /* Transform component hold a reference to the game object they belong to.
     * To do so they need an external handle which is created after the game
     * object itself. So the external handle must take care of this. */
    if (isValid())
        mHandleList->get(mHandleIndex, mGeneration).transform.gameObject = *this;
}

GameObjectEH::GameObjectEH()
    : GameObjectEH{ nullptr, 0, 0 } { }
