#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <limits>

struct BoundingBox
{
private:
	glm::vec3 mMin{ std::numeric_limits<float>::infinity() };
	glm::vec3 mMax{ -std::numeric_limits<float>::infinity() };

	void updateMinMax(const glm::vec3& newPoint);

public:

	BoundingBox();
	/**
	 * Creates a new bounding box for the given points.
	 * @param points a vector of points that will be contained in the bounding box.
	 */
	BoundingBox(const std::vector<glm::vec3>& points);
	
	/**
	 * Creates a new bounding box given two points representing its extent.
	 * @param minPoint the point with the min values for x, y and z
	 * @param maxPoint the point with the min values for x, y and z
	 */
	BoundingBox(const glm::vec3& minPoint, const glm::vec3& maxPoint);

	const glm::vec3& getMin() const;

	const glm::vec3& getMax() const;

	const glm::vec3 getExtent() const;

	const glm::vec3 getCenter() const;

	void extend(const glm::vec3& newPoint);

	void extend(const BoundingBox& boundingBox);

	BoundingBox transformed(const glm::mat4& transformationMatrix) const;
};

