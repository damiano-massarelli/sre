#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <vector>
#include <string.h>

struct Bone
{
	Bone() = default;

	std::int32_t parent = -1;
	std::vector<std::int32_t> children;
	std::string name;

	/** Coverts from local space to bone space */
	glm::mat4 offset;

	glm::mat4 toParentSpace;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

