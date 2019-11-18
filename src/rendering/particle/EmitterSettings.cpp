#include "EmitterSettings.h"
#include "gameobject/Transform.h"
#include "gameobject/GameObject.h"
#include <cstdlib>

float EmitterSettings::randVal(float min, float max)
{
	float factor = (float)rand() / RAND_MAX;
	return min + (max - min) * factor;
}

glm::vec3 EmitterSettings::randVal(const glm::vec3& min, const glm::vec3& max)
{
	return glm::vec3{
		randVal(min.x, max.x),
		randVal(min.y, max.y),
		randVal(min.z, max.z)
	};
}

void EmitterSettings::setUp(const GameObjectEH& go, Particle& p)
{
	p.durationMillis = randVal(minDuration, maxDuration);

	Transform& transform = go->transform;
	p.position = randVal(transform.getPosition() + positionOffsetMin, transform.getPosition() + positionOffsetMax);

	glm::vec3 minVel = velocityMin;
	glm::vec3 maxVel = velocityMax;

	if (followEmitterDirection) {
		minVel = transform.modelToUpright() * minVel;
		maxVel = transform.modelToUpright() * maxVel;
	}

	p.velocity = randVal(minVel, maxVel);

	p.initialRotation = randVal(initialRotationMin, initialRotationMax);
	p.finalRotation = randVal(finalRotationMin, finalRotationMax);

	p.initialScale = randVal(initialScaleMin, initialScaleMax);
	p.finalScale = randVal(finalScaleMin, finalScaleMax);

	p.initialGravityScale = randVal(initialGravityScaleMin, initialGravityScaleMax);
	p.finalGravityScale = randVal(finalGravityScaleMin, finalGravityScaleMax);
}

