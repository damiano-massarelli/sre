#include "GeoMipMappingComponent.h"
#include "Engine.h"

glm::vec2 GeoMipMappingComponent::getPosition(std::uint32_t h, std::uint32_t v)
{
	float vPercent = (float)v / mVerticalVertex;
	float hPercent = (float)h / mHorizontalVertex;
	float vPos = -(mWidth / 2.0f) + vPercent * mWidth;
	float hPos = -(mDepth / 2.0f) + hPercent * mDepth;
	return glm::vec2(hPos, vPos);
}

GeoMipMappingComponent::GeoMipMappingComponent(const GameObjectEH& go, std::uint32_t width, float depth, std::uint32_t vVertex, std::uint32_t hVertex)
	: Component{ go }
{

}

std::vector<std::uint32_t> GeoMipMappingComponent::compute(std::vector<std::uint32_t> indices, std::uint32_t h, std::uint32_t v, std::uint32_t hOff, std::uint32_t vOff)
{
	auto pos3D = Engine::renderSys.camera->transform.getPosition();

	glm::vec2 camPos = glm::vec2{ pos3D.x, pos3D.z };
	glm::vec2 center = (getPosition(h, v) + getPosition(h + hOff, v + vOff)) / 2.0f;

	if (glm::distance(camPos, center) < 150 && hOff != 1 && vOff != 1) {
		std::uint32_t hHalf = hOff / 2;
		std::uint32_t vHalf = vOff / 2;

		compute(indices, h, v, hHalf, vHalf);
		compute(indices, h + hHalf, v, hHalf, vHalf);

		compute(indices, h, v + vHalf, hHalf, vHalf);
		compute(indices, h + hHalf, v + vHalf, hHalf, vHalf);
	}
	else {
		indices.insert(indices.end(), {
			// first tri
			h + (mHorizontalVertex + 1) * v,
			h + (mHorizontalVertex + 1) * (v + vOff),
			h + hOff + (mHorizontalVertex + 1) * v,

			// second tri
			h + hOff + (mHorizontalVertex + 1) * v,
			h + (mHorizontalVertex + 1) * (v + vOff),
			h + hOff + (mHorizontalVertex + 1) * (v + vOff),
		});
	}
}

void GeoMipMappingComponent::geomipmap()
{
	std::vector<std::uint32_t> indices;

	std::uint32_t hHalf = mHorizontalVertex / 2;
	std::uint32_t vHalf = mVerticalVertex / 2;

	compute(indices, 0, 0, hHalf, vHalf);
	compute(indices, hHalf, 0, hHalf, vHalf);
	compute(indices, 0, vHalf, hHalf, vHalf);
	compute(indices, hHalf, vHalf, hHalf, vHalf);

	gameObject
}
