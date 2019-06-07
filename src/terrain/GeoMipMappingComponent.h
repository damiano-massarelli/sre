#pragma once

#include "Component.h"
#include "GameObjectEH.h"
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

class GeoMipMappingComponent : public Component
{
private:
	float mWidth;
	float mDepth;

	std::uint32_t mVerticalVertex;
	std::uint32_t mHorizontalVertex;

	glm::vec2 getPosition(std::uint32_t h, std::uint32_t v);

public:
	GeoMipMappingComponent(const GameObjectEH& go, std::uint32_t width, float depth, std::uint32_t vVertex, std::uint32_t hVertex);

	std::vector<std::uint32_t> compute(std::vector<std::uint32_t> indices, std::uint32_t h, std::uint32_t v, std::uint32_t hOff, std::uint32_t vOff);

	void geomipmap();

	~GeoMipMappingComponent() = default;
};

