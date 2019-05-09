#ifndef FREECAMERACOMPONENT_H
#define FREECAMERACOMPONENT_H
#include "EventManager.h"
#include "GameObject.h"
#include "Component.h"

class FreeCameraComponent : public Component, public EventListener {
    private:
        CrumbPtr crumb;

        // first time registering mouse movements?
        bool mFirst = true;
        std::int32_t mLastMouseX;
        std::int32_t mLastMouseY;

    public:
        float heading = 0.0f;
        float pitch = 0.0f;

        float xMouseSensitivity = -0.004f;
        float yMouseSensitivity = -0.004f;
        float moveSpeed = 10.0f;

        /** is the camera following mouse and keyboard */
        bool tracking = true;

    public:
        FreeCameraComponent(const GameObjectEH& go);

        virtual void onEvent(SDL_Event e) override;
};

#endif // FREECAMERACOMPONENT_H
