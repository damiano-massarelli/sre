#ifndef GAMEOBJECTEH_H
#define GAMEOBJECTEH_H
#include "components/HandleList.h"
#ifdef SRE_DEBUG
#include <cassert>
#endif

class GameObject;

struct GameObjectEH : public ExternalHandle {
    HandleList<GameObject>* mHandleList = nullptr;

    GameObjectEH(HandleList<GameObject>* handleList, std::uint32_t index, std::uint32_t generation);

    GameObjectEH();

    GameObject* operator*() const {
#ifdef SRE_DEBUG
        assert(isValid());
#endif  // SRE_DEBUG
        return &(mHandleList->get(mHandleIndex, mGeneration));
    }

    GameObject* operator->() const {
#ifdef SRE_DEBUG
        assert(isValid());
#endif  // SRE_DEBUG
        return &(mHandleList->get(mHandleIndex, mGeneration));
    }

    bool isValid() const {
        return mGeneration != 0 && mHandleList != nullptr && mHandleList->isValid(mHandleIndex, mGeneration);
    }

    explicit operator bool() const { return isValid(); }

    bool operator==(const GameObjectEH& rhs) const;

    bool operator!=(const GameObjectEH& rhs) const;
};

#endif  // GAMEOBJECTEH_H
