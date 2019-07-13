#pragma once
#include <glm/glm.hpp>

struct Particle
{
	friend class ParticleEmitter;

	glm::vec3 position = glm::vec3{ 0.0f };
	glm::vec3 velocity = glm::vec3{ 0.0f };
	float gravityScale = 1.0f;
	float rotation = 0.0f;
	float scale = 1.0f;
	float durationMillis = 1000.0f;
	float elapsedTime = 0.0f;
};

