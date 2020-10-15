#pragma once

#include "components/Component.h"
#include "gameobject/GameObjectEH.h"
#include "geometry/Frustum.h"
#include <array>
#include <glm/vec3.hpp>

/**
 * Component for GameObject%s that can act as cameras.
 * If a GameObject is to be used as a camera, this Component
 * must be attached to it.
 */
class CameraComponent : public Component {
private:
    float mFov;
    float mNearPlaneDistance = 0.1f;
    float mFarPlaneDistance = 1000.f;

    std::array<glm::vec3, 8> mFrustumVertices;

    /**
     * Calculates the vertices of the view frustum and
     * stores them in mFrustumVertices.
     */
    void calculateFrustumVertices();

public:
    CameraComponent(const GameObjectEH& go, float fov, float nearPlaneDistance, float farPlaneDistance);

    /**
     * Sets the vertical FOV.
     * @param fov the vertical fov of the camera
     */
    void setFOV(float fov);

    /**
     * @return the vertical fov.
     */
    float getFOV() const;

    /**
     * @return the distance of the near plane
     */
    float getNearPlaneDistance() const;

    /**
     * @return the distance of the far plane.
     */
    float getFarPlaneDistance() const;

    /**
     * Calculates and return the view frustum of the camera.
     * The vertices of the frustum are in world space.
     *
     * @return the view Frustum for this camera.
     */
    Frustum getViewFrutsum() const;

    virtual ~CameraComponent() = default;
};
