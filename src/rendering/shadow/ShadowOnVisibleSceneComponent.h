#pragma once
#include "Component.h"
#include "GameObjectEH.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include <array>
#include <glm/glm.hpp>
#include "EventManager.h"
/**
 * Moves the light attached to this component
 * so that the scene framed by the camera always has
 * shadows. The dimensions of the shadow box are
 * also varied so that the the shadow box is optimized
 * to contain as much of the visible scene as possible.
 *
 * Based on ThinMatrix tutorial.
 */
class ShadowOnVisibleSceneComponent :
	public Component, public EventListener
{
private:
	float nearWidth, nearHeight, farWidth, farHeight;
	CrumbPtr mCrumb;

public:
	float shadowDistance = 0.3f;

	ShadowOnVisibleSceneComponent(const GameObjectEH& go);

	void computeWidthsAndHeights();

	virtual void onEvent(SDL_Event e) override;

	std::array<glm::vec3, 8> calculateFrustumVertices(const glm::vec3& up, const glm::vec3& right,
		const glm::vec3& centerNear, glm::vec3& centerFar);

	~ShadowOnVisibleSceneComponent() = default;

};

