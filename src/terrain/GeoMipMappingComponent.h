#pragma once

#include "Component.h"
#include "GameObjectEH.h"
#include "EventListener.h"
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <future>

class GeoMipMappingComponent : public Component, EventListener
{
private:
	std::future<std::vector<std::uint32_t>> mResult;

	float mWidth;
	float mDepth;

	std::uint32_t mVerticalVertex;
	std::uint32_t mHorizontalVertex;

	glm::vec2 getPosition(std::uint32_t h, std::uint32_t v);

public:
	GeoMipMappingComponent(const GameObjectEH& go, float width, float depth, std::uint32_t hVertex, std::uint32_t vVertex);

	inline void compute(std::vector<std::uint32_t>& indices, std::uint32_t h, std::uint32_t v, std::uint32_t hOff, std::uint32_t vOff);

	std::vector<std::uint32_t> geomipmap();

	virtual void onEvent(SDL_Event e) override;

	~GeoMipMappingComponent() = default;
};

