#include "Frustum.h"

Frustum::Frustum(const std::array<glm::vec3, 8>& vertices)
    : mVertices{ vertices }
    , mFarPlane{ vertices[2], vertices[0], vertices[3] }
    , mNearPlane{ vertices[4], vertices[6], vertices[5] }
    , mTopPlane{ vertices[0], vertices[2], vertices[6] }
    , mBottomPlane{ vertices[7], vertices[5], vertices[1] }
    , mRightPlane{ vertices[0], vertices[4], vertices[7] }
    , mLeftPlane{ vertices[2], vertices[1], vertices[5] } { }

std::array<Plane, 6> Frustum::getPlanes() const {
    return { mTopPlane, mRightPlane, mBottomPlane, mLeftPlane, mFarPlane, mNearPlane };
}

const std::array<glm::vec3, 8>& Frustum::getVertices() const { return mVertices; }
