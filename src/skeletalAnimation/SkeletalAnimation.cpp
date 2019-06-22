#include "SkeletalAnimation.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

SkeletalAnimation::SkeletalAnimation(float duration)
{
	mDuration = duration;
}

float SkeletalAnimation::getDuration() const
{
	return mDuration;
}

bool SkeletalAnimation::isEmpty() const
{
	return mDuration < 0.00001f;
}

std::vector<glm::mat4> SkeletalAnimation::getAt(float ticks, const std::vector<Bone>& skeleton) const
{
	if (loopDirection == LoopDirection::REPEAT) ticks = std::fmod(ticks, mDuration);
	if (loopDirection == LoopDirection::BOUNCE) ticks = (static_cast<int>(ticks / mDuration) % 2 == 0 ?
		std::fmod(ticks, mDuration) : mDuration - std::fmod(ticks, mDuration));


	std::vector<glm::mat4> transforms;
	for (std::size_t boneIndex = 0; boneIndex < skeleton.size(); boneIndex++) {
		const auto& originalBone = skeleton[boneIndex];

		glm::vec3 position = originalBone.position;
		glm::quat rotation = originalBone.rotation;
		glm::vec3 scale = originalBone.scale;

		if (boneIndex < mBoneKeyFrames.size()) { // go on only if the key frame is available
			const auto& boneKey = mBoneKeyFrames[boneIndex];

			// positions
			std::uint32_t positionIndex = 0;
			for (positionIndex = 0; positionIndex < boneKey.positionKeys.size(); ++positionIndex)
				if (ticks < boneKey.positionKeys[positionIndex]) break;

			if (positionIndex == 0) position = originalBone.position; // default position;
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

			if (scalingIndex == 0) scale = originalBone.scale; // default scale;
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

			if (rotationIndex == 0) rotation = originalBone.rotation; // default rotation;
			else if (rotationIndex == boneKey.rotationKeys.size()) rotation = boneKey.rotations[rotationIndex - 1];
			else {
				auto startTime = boneKey.rotationKeys[rotationIndex - 1];
				auto finishTime = boneKey.rotationKeys[rotationIndex];
				auto start = boneKey.rotations[rotationIndex - 1];
				auto finish = boneKey.rotations[rotationIndex];
				rotation = glm::slerp(start, finish, (ticks - startTime) / (finishTime - startTime));
			}
		}

		// build the transformation matrix
		glm::mat4 m2w = glm::translate(glm::mat4{ 1.0f }, position);
		m2w *= glm::toMat4(rotation);
		m2w = glm::scale(m2w, scale);
		transforms.push_back(m2w);
	}

	return transforms;
}
