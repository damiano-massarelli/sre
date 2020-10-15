#pragma once
#include "Plane.h"
#include <array>
#include <glm/glm.hpp>

/**
 * A frustum.
 * A Frustum is composed of 6 planes. The space inside these
 * Plane%s represents the extent of the frustum.
 */
struct Frustum {
private:
    Plane mTopPlane;
    Plane mRightPlane;
    Plane mBottomPlane;
    Plane mLeftPlane;
    Plane mFarPlane;
    Plane mNearPlane;

    std::array<glm::vec3, 8> mVertices;

public:
    /**
     * Creates a new Frustum from its 8 vertices.
     * @param vertices
     */
    Frustum(const std::array<glm::vec3, 8>& vertices);

    /**
     * Returns the Plane%s composing this Frustum.
     * The Planes are in the following order:
     * top, right, bottom, left, far, near.
     * @return the Plane%s of this Frustum.
     */
    std::array<Plane, 6> getPlanes() const;

    const std::array<glm::vec3, 8>& getVertices() const;
};
