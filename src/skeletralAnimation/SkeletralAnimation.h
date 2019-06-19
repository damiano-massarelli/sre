#pragma once

#include "Bone.h"
#include <vector>
#include <glm/glm.hpp>
#include<glm/gtx/quaternion.hpp>

struct BoneKeyframes {
	int boneIndex = -1;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> scalings;
	std::vector<glm::quat> rotations;

	std::vector<float> positionKeys;
	std::vector<float> scalingKeys;
	std::vector<float> rotationKeys;
};

class SkeletralAnimation
{
public:
	std::vector<BoneKeyframes> boneKeyframes;

	std::vector<glm::mat4> getAt(float ticks, const std::vector<Bone>& skeleton);

	SkeletralAnimation() = default;
};

