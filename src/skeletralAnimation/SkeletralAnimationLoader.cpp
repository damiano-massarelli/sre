#include "SkeletralAnimationLoader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

SkeletralAnimation SkeletralAnimationLoader::fromAssimpScene(const aiScene* scene, const std::map<std::string, std::uint32_t>& boneName2index)
{
	SkeletralAnimation loadedAnimation;
	loadedAnimation.boneKeyframes.resize(boneName2index.size());

	for (std::uint32_t i = 0; i < scene->mNumAnimations; ++i) {
		aiAnimation* animation = scene->mAnimations[i];
		for (std::uint32_t c = 0; c < animation->mNumChannels; ++c) {
			aiNodeAnim* channel = animation->mChannels[c];
			auto boneIndexEntry = boneName2index.find(channel->mNodeName.C_Str());
			if (boneIndexEntry == boneName2index.end()) std::cerr << "cannot find bone named " << channel->mNodeName.C_Str() << "\n";
			std::uint32_t boneIndex = boneIndexEntry->second;

			std::cout << channel->mNodeName.C_Str() << " " << boneIndex << "\n";
			BoneKeyframes& bone = loadedAnimation.boneKeyframes[boneIndex];

			// load positions 
			for (std::uint32_t p = 0; p < channel->mNumPositionKeys; ++p) {
				aiVectorKey& key = channel->mPositionKeys[p];
				aiVector3D& position = key.mValue;
				bone.positionKeys.push_back(key.mTime);
				bone.positions.push_back(glm::vec3{ position.x, position.y, position.z });
			}

			// load scalings
			for (std::uint32_t s = 0; s < channel->mNumScalingKeys; ++s) {
				aiVectorKey& key = channel->mScalingKeys[s];
				aiVector3D& scaling = key.mValue;
				bone.scalingKeys.push_back(key.mTime);
				bone.scalings.push_back(glm::vec3{ scaling.x, scaling.y, scaling.z });
			}

			// load rotations
			for (std::uint32_t r = 0; r < channel->mNumRotationKeys; ++r) {
				aiQuatKey& key = channel->mRotationKeys[r];
				aiQuaternion& rotation = key.mValue;
				glm::quat convRotation{ rotation.w, rotation.x, rotation.y, rotation.z };
				bone.rotationKeys.push_back(key.mTime);
				bone.rotations.push_back(convRotation);
			}

		}
	}

	return loadedAnimation;
}

SkeletralAnimationLoader::SkeletralAnimationLoader()
{
}


SkeletralAnimationLoader::~SkeletralAnimationLoader()
{
}
