#include "Intersections.h"
#include "BoundingBox.h"
#include "Frustum.h"
#include "Plane.h"
#include <glm/glm.hpp>

IntersectionTestResult operator&(IntersectionTestResult r1, IntersectionTestResult r2) {
    return static_cast<IntersectionTestResult>(static_cast<std::int8_t>(r1) & static_cast<std::int8_t>(r2));
}

IntersectionTestResult planeBoxIntersection(const Plane& plane, const BoundingBox& box) {
    // this algorithm is taken from real-time rendering 3rd ed.
    const glm::vec3 center = box.getCenter();
    const glm::vec3 diagonal = box.getDiagonal();
    const glm::vec3 normal = plane.getNormal();

    const glm::vec3 absNormal = glm::abs(normal);

    const float e = glm::dot(diagonal, absNormal);
    const float centerPlaneDistance = glm::dot(center, normal) - plane.getDistanceFromOrigin();

    if (centerPlaneDistance - e > 0.0f) {
        return IntersectionTestResult::INSIDE;
    } else if (centerPlaneDistance + e < 0.0f) {
        return IntersectionTestResult::OUTSIDE;
    }

    return IntersectionTestResult::OVERLAP;
}

IntersectionTestResult boxFrustumIntersection(const BoundingBox& box, const Frustum& frustum) {
    IntersectionTestResult result = IntersectionTestResult::INSIDE;
    for (const Plane& plane : frustum.getPlanes()) {
        const IntersectionTestResult partial = planeBoxIntersection(plane, box);
        if (partial == IntersectionTestResult::OUTSIDE)
            return IntersectionTestResult::OUTSIDE;
        result = result & partial;
    }

    return result;
}
