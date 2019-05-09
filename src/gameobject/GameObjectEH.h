#ifndef GAMEOBJECTEH_H
#define GAMEOBJECTEH_H
#include "HandleList.h"

class GameObject;

struct GameObjectEH : public ExternalHandle {
    private:
        HandleList<GameObject>* mHandleList = nullptr;

    public:
        GameObjectEH(HandleList<GameObject>* handleList, std::uint32_t index, std::uint32_t generation);

        GameObjectEH();

        inline GameObject* operator->() const {
            return &mHandleList->get(mHandleIndex, mGeneration);
        }

        bool isValid() const {
            return mGeneration != 0 && mHandleList != nullptr && mHandleList->isValid(mHandleIndex, mGeneration);
        }

        operator bool() const {
            return isValid();
        }
};

#endif // GAMEOBJECTEH_H