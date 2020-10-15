#include "cameras/FreeCameraComponent.h"
#include "Engine.h"
#include "cameras/CameraComponent.h"
#include "events/EventManager.h"

FreeCameraComponent::FreeCameraComponent(const GameObjectEH& go)
    : Component(go) {
    crumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
    Engine::eventManager.addListenerFor(SDL_MOUSEMOTION, crumb.get());
    Engine::eventManager.addListenerFor(SDL_KEYDOWN, crumb.get());

    SDL_SetRelativeMouseMode(SDL_TRUE);

    syncWithTransform();

    /* Adds the camera component if this go does not have one */
    if (!go->getComponent<CameraComponent>()) {
        std::shared_ptr<CameraComponent> cameraComponent = std::make_shared<CameraComponent>(go, 0.785f, 0.1f, 1000.0f);
        go->addComponent(cameraComponent);
    }
}

void FreeCameraComponent::syncWithTransform() {
    // FIXME sometimes this does not work
    auto angles = glm::eulerAngles(gameObject->transform.getRotation());
    constexpr float pi = glm::pi<float>();
    if (angles.z == pi) {
        angles.x = -pi + angles.x;
        angles.y = pi - angles.y;
    } else if (angles.z == -pi) {
        angles.x = pi + angles.x;
        angles.y = pi - angles.y;
    }
    heading = angles.y;
    pitch = angles.x;
}

void FreeCameraComponent::onEvent(SDL_Event e) {
    Transform& transform = gameObject->transform;

    if (e.type == EventManager::ENTER_FRAME_EVENT) {
        if (!tracking)
            return;

        float delta = (*(static_cast<float*>(e.user.data1))) / 1000.0f;
        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        glm::vec3 camLookDirection = transform.forward();
        glm::vec3 camRight = transform.right();

        if (keys[SDL_SCANCODE_W])
            transform.moveBy(camLookDirection * delta * mMoveSpeed * mCameraSensitivity);
        if (keys[SDL_SCANCODE_S])
            transform.moveBy(-camLookDirection * delta * mMoveSpeed * mCameraSensitivity);
        if (keys[SDL_SCANCODE_D])
            transform.moveBy(-camRight * delta * mMoveSpeed * mCameraSensitivity);
        if (keys[SDL_SCANCODE_A])
            transform.moveBy(camRight * delta * mMoveSpeed * mCameraSensitivity);

        if (keys[SDL_SCANCODE_Q])
            transform.moveBy(glm::vec3{ 0.f, -1.f, 0.f } * delta * mMoveSpeed * mCameraSensitivity);
        if (keys[SDL_SCANCODE_E])
            transform.moveBy(glm::vec3{ 0.f, 1.f, 0.f } * delta * mMoveSpeed * mCameraSensitivity);

        if (glm::any(glm::notEqual(transform.getRotation(), mOldOrientation))) {
            syncWithTransform();
            transform.setRotation(glm::vec3{ pitch, heading, 0.0f });
            mOldOrientation = transform.getRotation();
        }
    }
    if (e.type == SDL_MOUSEMOTION && tracking) {
        heading += e.motion.xrel * mXMouseSensitivity * mCameraSensitivity;
        pitch -= e.motion.yrel * mYMouseSensitivity * mCameraSensitivity;
        transform.setRotation(glm::vec3{ pitch, heading, 0.0f });
        mOldOrientation = transform.getRotation();
    }
    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_t || e.key.keysym.sym == SDLK_ESCAPE)) {
        tracking = !tracking;
        SDL_SetRelativeMouseMode(static_cast<SDL_bool>(tracking));
    }
}

void FreeCameraComponent::setCameraSensitivity(float sensitivity) {
    const float clampedFactor = std::clamp(sensitivity, MIN_SENSITIVITY_FACTOR, MAX_SENSITIVITY_FACTOR);
    mCameraSensitivity = clampedFactor;
}
