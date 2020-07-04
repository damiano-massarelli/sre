#ifndef FREECAMERACOMPONENT_H
#define FREECAMERACOMPONENT_H
#include "events/EventManager.h"
#include "gameobject/GameObject.h"
#include "components/Component.h"

/**
  * A free camera can be moved around freely using WASD and rotated around
  * the y and x axis using the mouse. By pressing T it is possible to toggle
  * the state: when the camera is tracking the mouse is controlled by the camera,
  * when the camera is not tracking pressing WASD wont move it and the control of the
  * mouse is given back to the user */
class FreeCameraComponent : public Component, public EventListener {
    private:
        CrumbPtr crumb;

        // First time registering mouse movements?
        bool mFirst = true;
        std::int32_t mLastMouseX;
        std::int32_t mLastMouseY;

		glm::quat mOldOrientation;

        // Sensitivity
        float mXMouseSensitivity = -0.004f;
        float mYMouseSensitivity = -0.004f;
        float mMoveSpeed = 70.0f;

        float mCameraSensitivity = 1.f;

        static constexpr float MIN_SENSITIVITY_FACTOR = 0.1f;
        static constexpr float MAX_SENSITIVITY_FACTOR = 5.f;

    public:
        float heading = 0.0f;
        float pitch = 0.0f;

        /** is the camera following mouse and keyboard */
        bool tracking = true;

        FreeCameraComponent(const GameObjectEH& go);

		/**
		 * Sync the orientation with that of the GameObject transform.
		 * When the rotation of the GameObject this Component handles
		 * changes, this method should be called.
		 */
		void syncWithTransform();

        virtual void onEvent(SDL_Event e) override;

        void setCameraSensitivity(float sensitivity);

        inline float getCameraSensitivity() const { return mCameraSensitivity; }
};

#endif // FREECAMERACOMPONENT_H
