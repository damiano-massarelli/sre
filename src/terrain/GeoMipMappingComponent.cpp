#include "terrain/GeoMipMappingComponent.h"
#include "Engine.h"
#include "events/EventManager.h"
#include <future>


glm::vec2 GeoMipMappingComponent::getPosition(std::uint32_t h, std::uint32_t v)
{
	float vPercent = (float)v / mVerticalVertex;
	float hPercent = (float)h / mHorizontalVertex;
	float vPos = -(mWidth / 2.0f) + vPercent * mWidth;
	float hPos = -(mDepth / 2.0f) + hPercent * mDepth;
	return glm::vec2(hPos, vPos);
}

GeoMipMappingComponent::GeoMipMappingComponent(const GameObjectEH& go, float width, float depth, std::uint32_t hVertex, std::uint32_t vVertex)
	: Component{ go }, mWidth{ width }, mDepth{ depth }, mVerticalVertex{ vVertex }, mHorizontalVertex{ hVertex }
{
	Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, false);

	// starts async computation
	mResult = std::async(std::launch::async, [this]() { return geomipmap(); });
}

void GeoMipMappingComponent::compute(std::vector<std::uint32_t>& indices, std::uint32_t h, std::uint32_t v, std::uint32_t hOff, std::uint32_t vOff)
{
	auto pos3D = Engine::renderSys.camera->transform.getPosition();

	glm::vec2 camPos = glm::vec2{ pos3D.x, pos3D.z };
	glm::vec2 topLeft = getPosition(h, v);
	glm::vec2 bottomRight = getPosition(h + hOff, v + vOff);
	glm::vec2 center = (topLeft + bottomRight) / 2.0f;

	if (glm::distance(camPos, center) < glm::distance(topLeft, bottomRight) *2  && hOff != 1 && vOff != 1) {
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

std::vector<std::uint32_t> GeoMipMappingComponent::geomipmap()
{
	std::vector<std::uint32_t> indices;

	std::uint32_t hHalf = mHorizontalVertex / 2;
	std::uint32_t vHalf = mVerticalVertex / 2;

	compute(indices, 0, 0, hHalf, vHalf);
	compute(indices, hHalf, 0, hHalf, vHalf);
	compute(indices, 0, vHalf, hHalf, vHalf);
	compute(indices, hHalf, vHalf, hHalf, vHalf);

	return indices;
}

void GeoMipMappingComponent::onEvent(SDL_Event e)
{
	if (mResult._Is_ready()) {
		auto indices = mResult.get();
		std::uint32_t ebo = gameObject->getMeshes()[0].getEbo();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mResult = std::async(std::launch::async, [this]() { return geomipmap(); });
	}
	
}
