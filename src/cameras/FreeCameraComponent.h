#ifndef FREECAMERACOMPONENT_H
#define FREECAMERACOMPONENT_H
#include "EventManager.h"
#include "GameObject.h"
#include "Component.h"

/**
  * A free camera can be moved around freely using WASD and rotated around
  * the y and x axis using the mouse. By pressing T it is possible to toggle
  * the state: when the camera is tracking the mouse is controlled by the camera,
  * when the camera is not tracking pressing WASD wont move it and the control of the
  * mouse is given back to the user */
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
