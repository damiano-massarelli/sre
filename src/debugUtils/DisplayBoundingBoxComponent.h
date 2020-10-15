#pragma once

#include "components/Component.h"
#include "events/EventManager.h"
#include "gameobject/GameObject.h"
#include <memory>

class EventListener;

/**
 * When this component is attached to a GameObject its bounding box will be
 * displayed. This component should only be used for debugging purposes and can
 * be relatively slow.
 */
class DisplayBoundingBoxComponent : public Component, EventListener {
private:
    /**
     * The GameObject used to represent the BoundingBox
     */
    GameObjectEH mBoundingBox;

    CrumbPtr mEnterFrameCrumb;

public:
    DisplayBoundingBoxComponent(const GameObjectEH& go, const glm::vec3& bbColor = glm::vec3{ 1.0f, 0.0f, 0.0f });

    virtual void onEvent(SDL_Event e) override;

    virtual ~DisplayBoundingBoxComponent();
};
