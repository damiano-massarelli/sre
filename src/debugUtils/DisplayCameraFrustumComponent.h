#pragma once

#include "components/Component.h"
#include "events/EventManager.h"
#include "gameobject/GameObject.h"
#include "rendering/materials/PropMaterial.h"
#include <memory>

class EventListener;

/**
 * When this component is attached to a GameObject its view Frustum will be
 * displayed. The GameObject must have a CameraComponent already attached to it.
 * This component should only be used for debugging purposes and
 * can be relatively slow.
 */
class DisplayCameraFrustumComponent : public Component, EventListener {
private:
    /**
     * The GameObject used to contain the Frustum
     */
    GameObjectEH mContainer;

    std::shared_ptr<PropMaterial> mMaterial;

    CrumbPtr mEnterFrameCrumb;

public:
    DisplayCameraFrustumComponent(const GameObjectEH& go, const glm::vec3& color = glm::vec3{ 0.0f, 1.0f, 0.0f });

    virtual void onEvent(SDL_Event e) override;

    virtual ~DisplayCameraFrustumComponent() = default;
};
