#pragma once
#include <glm/glm.hpp>

struct Particle
{
	friend class ParticleEmitter;

	glm::vec3 position = glm::vec3{ 0.0f };
	glm::vec3 velocity = glm::vec3{ 0.0f };
	float initialGravityScale = 1.0f;
	float finalGravityScale = 1.0f;

	float initialRotation = 0.0f;
	float finalRotation = 0.0f;

	float initialScale = 1.0f;
	float finalScale = 1.0f;

	float durationMillis = 1000.0f;
	float elapsedTime = 0.0f;
};

