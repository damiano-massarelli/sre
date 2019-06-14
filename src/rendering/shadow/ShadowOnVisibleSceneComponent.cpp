#include "ShadowOnVisibleSceneComponent.h"
#include "Engine.h"
#include <cmath>
#include <algorithm>
#include "MeshCreator.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "PropMaterial.h"


ShadowOnVisibleSceneComponent::ShadowOnVisibleSceneComponent(const GameObjectEH& go)
	: Component{ go }
{
	mCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
	computeWidthsAndHeights();
}

void ShadowOnVisibleSceneComponent::computeWidthsAndHeights()
{
	float aspectRatio = Engine::renderSys.getScreenWidth() / (float)Engine::renderSys.getScreenHeight();

	float fovy = Engine::renderSys.getVerticalFov() / 2;
	nearHeight = std::tan(fovy) * Engine::renderSys.getNearPlane();
	farHeight = std::tan(fovy) * Engine::renderSys.getFarPlane() * shadowDistance;

	farWidth = farHeight * aspectRatio;
	nearWidth = nearHeight * aspectRatio;

	std::cout << farWidth << " " << farHeight << "\n";
}

void ShadowOnVisibleSceneComponent::onEvent(SDL_Event e)
{
	auto& renderSys = Engine::renderSys;
	auto& cameraTransform = renderSys.camera->transform;

	// find center for far and near plane
	glm::vec3 toNear = cameraTransform.forward() * renderSys.getNearPlane();
	glm::vec3 toFar = cameraTransform.forward() * renderSys.getFarPlane() * shadowDistance;

	glm::vec3 centerNear = toNear + cameraTransform.getPosition();
	glm::vec3 centerFar = toFar + cameraTransform.getPosition();

	auto verices = calculateFrustumVertices(cameraTransform.up(), cameraTransform.right(), centerNear, centerFar);

	auto min = verices[0];
	auto max = verices[0];

	for (const auto& v : verices) {
		min = glm::min(min, v);
		max = glm::max(max, v);
	}

	auto center = (max + min) / 2.0f;
	auto bb = max - min;

	renderSys.shadowMappingSettings.width = bb.x;
	renderSys.shadowMappingSettings.depth = bb.y;
	renderSys.shadowMappingSettings.height = bb.z;

	gameObject->transform.setPosition(center);
}

std::array<glm::vec3, 8> ShadowOnVisibleSceneComponent::calculateFrustumVertices(const glm::vec3& up, const glm::vec3& right, const glm::vec3& centerNear, glm::vec3& centerFar)
{
	glm::vec3 toFarTopRight = up * farHeight + right * farWidth;
	glm::vec3 toFarBottomLeft = -toFarTopRight;
	glm::vec3 toFarTopLeft = up * farHeight - right * farWidth;
	glm::vec3 toFarBottomRight = -toFarTopLeft;

	glm::vec3 toNearTopRight = up * nearHeight + right * nearWidth;
	glm::vec3 toNearBottomLeft = -toNearTopRight;
	glm::vec3 toNearTopLeft = up * nearHeight - right * nearHeight;
	glm::vec3 toNearBottomRight = -toNearTopLeft;

	return std::array<glm::vec3, 8> {
		centerFar + toFarTopRight,
		centerFar + toFarBottomLeft,
		centerFar + toFarTopLeft,
		centerFar + toFarBottomRight,

		centerNear + toNearTopRight,
		centerNear + toNearBottomLeft,
		centerNear + toNearTopLeft,
		centerNear + toNearBottomRight
	};
}

