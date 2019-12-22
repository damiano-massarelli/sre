#pragma once

/**
 * This file contains some utility functions
 * to check for intersections between shapes.
 */ 

#include <cstdint>

struct Plane;
struct BoundingBox;
struct Frustum;

/**
 * The possible results of an intersection test.
 */
enum class IntersectionTestResult : std::int8_t {
	INSIDE		= 0x03, // so that INSIDE & OVERLAP = OVERLAP and (INSIDE/OVERLAP) & OUTSIDE = OUTSIDE
	OVERLAP		= 0x01,
	OUTSIDE		= 0
};

/**
 * Checks if a BoundingBox intersects a Plane.
 * The normal of the plane indicates the inside of the space partitioned by it.
 * @param plane the plane
 * @param the bounding box
 * @return whether the box is outside, inside or it's overlapping the plane
 */
IntersectionTestResult planeBoxIntersection(const Plane& plane, const BoundingBox& box);

/**
 * Checks if a BoundingBox intersects a Frustum.
 * @param box the bounding box
 * @param frustum the frustum
 * @return whether the box is outside, inside or it's overlapping the frustum
 */
IntersectionTestResult boxFrustumIntersection(const BoundingBox& box, const Frustum& frustum);