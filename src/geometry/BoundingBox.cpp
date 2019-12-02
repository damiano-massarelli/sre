#include "BoundingBox.h"

void BoundingBox::updateMinMax(const glm::vec3 & newPoint)
{
	for (std::uint16_t j = 0; j < 3; ++j) {
		if (newPoint[j] <= mMin[j]) {
			mMin[j] = newPoint[j];
		}
		else if (newPoint[j] > mMax[j]) {
			mMax[j] = newPoint[j];
		}
	}
}

BoundingBox::BoundingBox()
{

}

BoundingBox::BoundingBox(const std::vector<glm::vec3>& points)
{
	if (points.size() == 0) {
		return;
	}

	mMin = points[0];
	mMax = points[0];

	for (std::size_t i = 1; i < points.size(); ++i) {
		updateMinMax(points[i]);
	}
}

BoundingBox::BoundingBox(const glm::vec3 & minPoint, const glm::vec3 & maxPoint)
{
	mMin = glm::min(minPoint, maxPoint);
	mMax = glm::max(minPoint, maxPoint);
}

const glm::vec3& BoundingBox::getMin() const
{
	return mMin;
}

const glm::vec3 & BoundingBox::getMax() const
{
	return mMax;
}

const glm::vec3 BoundingBox::getExtent() const
{
	return mMax - mMin;
}

const glm::vec3 BoundingBox::getCenter() const
{
	return (mMax + mMin) / 2.0f;
}

void BoundingBox::extend(const glm::vec3& newPoint)
{
	mMin = glm::min(mMin, newPoint);
	mMax = glm::max(mMax, newPoint);
}

void BoundingBox::extend(const BoundingBox& boundingBox)
{
	mMin = glm::min(mMin, boundingBox.getMin());
	mMax = glm::max(mMax, boundingBox.getMax());
}

BoundingBox BoundingBox::transformed(const glm::mat4& transformationMatrix) const
{
	glm::vec3 transformedMin = glm::vec3(transformationMatrix * glm::vec4{ mMin, 1.0f });
	glm::vec3 transformedMax = glm::vec3(transformationMatrix * glm::vec4{ mMax, 1.0f });

	// todo this could be optimized
	return BoundingBox(transformedMin, transformedMax);
}
