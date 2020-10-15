#include "Plane.h"

Plane::Plane(const glm::vec3& normal, const glm::vec3& pointOnPlane)
    : mNormal{ glm::normalize(normal) } {
    mDistanceFromOrigin = glm::dot(normal, pointOnPlane);
}

Plane::Plane(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    glm::vec3 d1 = v1 - v2;
    glm::vec3 d2 = v3 - v2;

    mNormal = glm::normalize(glm::cross(d1, d2));
    mDistanceFromOrigin = glm::dot(v1, mNormal);
}

const glm::vec3& Plane::getNormal() const { return mNormal; }

float Plane::getDistanceFromOrigin() const { return mDistanceFromOrigin; }
