#include "BoundingBox.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

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

bool BoundingBox::isValid() const
{
	for (int i = 0; i < 3; i++) {
		if (std::isinf(mMin[i])) {
			return false;
		}
	}

	for (int i = 0; i < 3; i++) {
		if (std::isinf(mMax[i])) {
			return false;
		}
	}

	return true;
}

BoundingBox BoundingBox::transformed(const glm::mat4& transformationMatrix) const
{
	// transforming an invalid bb would result in nans
	if (!isValid()) {
		return *this;
	}

	glm::vec3 translation = glm::vec3(transformationMatrix[3]);
	glm::mat3 transform = glm::mat3(transformationMatrix);

	glm::vec3 transformedMin{ 0.0f, 0.0f, 0.0f };
	glm::vec3 transformedMax{ 0.0f, 0.0f, 0.0f };

	for (std::uint32_t col = 0; col < 3; ++col) {
		for (std::uint32_t row = 0; row < 3; ++row) {
			const float currentElement = transform[col][row];
			if (currentElement >= 0.0f) {
				transformedMax[row] += mMax[col] * currentElement;
				transformedMin[row] += mMin[col] * currentElement;
			}
			else {
				transformedMax[row] += mMin[col] * currentElement;
				transformedMin[row] += mMax[col] * currentElement;
			}
		}
	}

	return BoundingBox(transformedMin + translation, transformedMax + translation);
}

std::ostream& operator<<(std::ostream& out, const BoundingBox& bb)
{
	std::cout << "BoundingBox(" << glm::to_string(bb.getMin()) << ", " << glm::to_string(bb.getMax()) << ")";
	return out;
}
