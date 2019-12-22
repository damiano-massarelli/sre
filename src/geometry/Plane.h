#pragma once

#include <glm/glm.hpp>

struct Plane
{
private:
	glm::vec3 mNormal{ 0.0f };
	float mDistanceFromOrigin = 0.0f;

public:
	Plane() = default;

	/**
	 * Creates a Plane given its normal and a point lying on it.
	 * @param normal the normal of the plane
	 * @param pointOnPlane a point lying on the plane
	 */
	Plane(const glm::vec3& normal, const glm::vec3& pointOnPlane);

	/**
	 * Creates a Plane given three points lying on it.
	 * The points are supposed to be in counterclockwise order
	 * so the plane normal will be (v1 - v2) x (v3 - v2)
	 * @param v1 first point
	 * @param v2 second point
	 * @param v3 third point
	 */
	Plane(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

	/**
	 * @return the normal of the Plane
	 */
	const glm::vec3& getNormal() const;

	/**
	 * @return the distance of the plane from the origin (aka W)
	 */
	float getDistanceFromOrigin() const;
};

