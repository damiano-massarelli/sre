#pragma once
#include "components/Component.h"
#include "gameobject/GameObjectEH.h"
#include "events/EventListener.h"
#include "events/EventListenerCrumb.h"
#include <array>
#include <glm/glm.hpp>
#include "events/EventManager.h"
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

	std::array<glm::vec3, 8> calculateFrustumVertices(const glm::vec3& up, const glm::vec3& right,
		const glm::vec3& centerNear, glm::vec3& centerFar);

	void computeWidthsAndHeights();

public:
	/** Multiplied by the current far plane, is the maximum distance
  * from which a shadow can be seen.
  * @see RenderSystem.getFarPlane() */
	float shadowDistance = 0.5f;

	ShadowOnVisibleSceneComponent(const GameObjectEH& go);

	virtual void onEvent(SDL_Event e) override;

	~ShadowOnVisibleSceneComponent() = default;
};

