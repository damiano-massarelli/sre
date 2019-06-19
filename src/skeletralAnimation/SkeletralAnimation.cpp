#include "SkeletralAnimation.h"
#include <glm/gtc/matrix_transform.hpp>

std::vector<glm::mat4> SkeletralAnimation::getAt(float ticks, const std::vector<Bone>& skeleton)
{
	std::vector<glm::mat4> transforms;
	for (std::size_t boneIndex = 0; boneIndex < boneKeyframes.size(); boneIndex++) {
		const auto& boneKey = boneKeyframes[boneIndex];
		const auto& originalBone = skeleton[boneIndex];

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;

		// positions
		std::uint32_t positionIndex = 0;
		for (positionIndex = 0; positionIndex < boneKey.positionKeys.size(); ++positionIndex) 
			if (ticks < boneKey.positionKeys[positionIndex]) break;
		
		if (positionIndex == 0) position = originalBone.position; // should return default position;
		else if (positionIndex == boneKey.positionKeys.size()) position = boneKey.positions[positionIndex - 1];
		else {
			auto startTime = boneKey.positionKeys[positionIndex - 1];
			auto finishTime = boneKey.positionKeys[positionIndex];
			auto start = boneKey.positions[positionIndex - 1];
			auto finish = boneKey.positions[positionIndex];
			position = glm::mix(start, finish, (ticks - startTime) / (finishTime - startTime));
		}

		// scaling
		std::uint32_t scalingIndex = 0;
		for (scalingIndex = 0; scalingIndex < boneKey.scalingKeys.size(); ++scalingIndex) 
			if (ticks < boneKey.scalingKeys[scalingIndex]) break;
		
		if (scalingIndex == 0) scale = originalBone.scale; // should return default position;
		else if (scalingIndex == boneKey.scalingKeys.size()) scale = boneKey.scalings[scalingIndex - 1];
		else {
			auto startTime = boneKey.scalingKeys[scalingIndex - 1];
			auto finishTime = boneKey.scalingKeys[scalingIndex];
			auto start = boneKey.scalings[scalingIndex - 1];
			auto finish = boneKey.scalings[scalingIndex];
			scale = glm::mix(start, finish, (ticks - startTime) / (finishTime - startTime));
		}

		// rotation
		std::uint32_t rotationIndex = 0;
		for (rotationIndex = 0; rotationIndex < boneKey.rotationKeys.size(); ++rotationIndex) 
			if (ticks < boneKey.rotationKeys[rotationIndex]) break;
		
		if (rotationIndex == 0) rotation = originalBone.rotation; // should return default position;
		else if (rotationIndex == boneKey.rotationKeys.size()) rotation = boneKey.rotations[rotationIndex - 1];
		else {
			auto startTime = boneKey.rotationKeys[rotationIndex - 1];
			auto finishTime = boneKey.rotationKeys[rotationIndex];
			auto start = boneKey.rotations[rotationIndex - 1];
			auto finish = boneKey.rotations[rotationIndex];
			rotation = glm::slerp(start, finish, (ticks - startTime) / (finishTime - startTime));
		}
		//rotation = originalBone.rotation;

		glm::mat4 m2w = glm::translate(glm::mat4{ 1.0f }, position);
		m2w *= glm::toMat4(rotation);
		m2w = glm::scale(m2w, scale);
		transforms.push_back(m2w);
	}

	return transforms;
}
