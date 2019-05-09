#include "FreeCameraComponent.h"
#include "Engine.h"
#include "EventManager.h"


FreeCameraComponent::FreeCameraComponent(const GameObjectEH& go) : Component(go)
{
    crumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
    Engine::eventManager.addListenerFor(SDL_MOUSEMOTION, crumb.get());
    Engine::eventManager.addListenerFor(SDL_KEYDOWN, crumb.get());

    SDL_SetRelativeMouseMode(SDL_TRUE);

    heading = glm::radians(180.0f);
}


void FreeCameraComponent::onEvent(SDL_Event e)
{
    Transform& transform = gameObject->transform;

    if (e.type == EventManager::ENTER_FRAME_EVENT) {
        transform.setRotation(glm::quat{glm::vec3{pitch, heading, 0.0f}});

        if (!tracking) return;
        float delta = (*(static_cast<float*>(e.user.data1)));
        const Uint8* keys = SDL_GetKeyboardState(nullptr);

        glm::vec3 camLookDirection = transform.forward();
        glm::vec3 camRight = transform.right();

        if (keys[SDL_SCANCODE_W])
            transform.moveBy(camLookDirection * delta * moveSpeed);
        if (keys[SDL_SCANCODE_S])
            transform.moveBy(-camLookDirection * delta * moveSpeed);
        if (keys[SDL_SCANCODE_D])
            transform.moveBy(camRight * delta * moveSpeed);
        if (keys[SDL_SCANCODE_A])
            transform.moveBy(-camRight * delta * moveSpeed);
    } if (e.type == SDL_MOUSEMOTION && tracking) {
        heading -= e.motion.xrel * xMouseSensitivity;
        pitch -= e.motion.yrel * yMouseSensitivity;
    } if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_t) {
        tracking = !tracking;
        SDL_SetRelativeMouseMode(static_cast<SDL_bool>(tracking));
    }

}
