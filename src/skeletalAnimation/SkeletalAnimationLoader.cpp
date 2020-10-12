#include "skeletalAnimation/SkeletalAnimationLoader.h"
#include <algorithm>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

SkeletalAnimation SkeletalAnimationLoader::fromAssimpScene(
    const aiScene* scene, const std::map<std::string, std::uint32_t>& boneName2index) {

    if (scene->mNumAnimations == 0) {
        std::cerr << "No animations found in scene\n";
        return SkeletalAnimation{ 0.0f };  // empty animation
    }

    // take ticks per second from first animation
    float ticksPerSecond = 0.0f;
    ticksPerSecond = static_cast<float>(scene->mAnimations[0]->mTicksPerSecond);
    if (ticksPerSecond == 0)
        ticksPerSecond = 24.0f;  // default to 24

    float duration = 0.0f;  // wait for the longest animation

    SkeletalAnimation loadedAnimation;
    loadedAnimation.mBoneKeyFrames.resize(boneName2index.size());

    // By default all animations in a single file are merged together.
    for (std::uint32_t i = 0; i < scene->mNumAnimations; ++i) {
        aiAnimation* animation = scene->mAnimations[i];
        float tps = static_cast<float>(animation->mTicksPerSecond);
        if (tps != ticksPerSecond)
            std::cerr << "animation : different ticks per second: " << animation->mName.C_Str() << ": " << tps
                      << " (used value: " << ticksPerSecond << ")\n";

        float animTime = static_cast<float>(animation->mDuration) / 1000.f;
        duration = std::max(duration, animTime);

        for (std::uint32_t c = 0; c < animation->mNumChannels; ++c) {
            aiNodeAnim* channel = animation->mChannels[c];
            auto boneIndexEntry = boneName2index.find(channel->mNodeName.C_Str());
            if (boneIndexEntry == boneName2index.end()) {
                std::cerr << "cannot find bone named " << channel->mNodeName.C_Str() << "\n";
                continue;
            }
            std::uint32_t boneIndex = boneIndexEntry->second;

            BoneKeyframes& bone = loadedAnimation.mBoneKeyFrames[boneIndex];

            // load positions
            for (std::uint32_t p = 0; p < channel->mNumPositionKeys; ++p) {
                aiVectorKey& key = channel->mPositionKeys[p];
                aiVector3D& position = key.mValue;
                bone.positionKeys.push_back(static_cast<float>(key.mTime / 1000.f));
                bone.positions.push_back(glm::vec3{ position.x, position.y, position.z });
            }

            // load scalings
            for (std::uint32_t s = 0; s < channel->mNumScalingKeys; ++s) {
                aiVectorKey& key = channel->mScalingKeys[s];
                aiVector3D& scaling = key.mValue;
                bone.scalingKeys.push_back(static_cast<float>(key.mTime / 1000.f));
                bone.scalings.push_back(glm::vec3{ scaling.x, scaling.y, scaling.z });
            }

            // load rotations
            for (std::uint32_t r = 0; r < channel->mNumRotationKeys; ++r) {
                aiQuatKey& key = channel->mRotationKeys[r];
                aiQuaternion& rotation = key.mValue;
                glm::quat convRotation{ rotation.w, rotation.x, rotation.y, rotation.z };
                bone.rotationKeys.push_back(static_cast<float>(key.mTime / 1000.f));
                bone.rotations.push_back(convRotation);
            }
        }
    }

    loadedAnimation.mDuration = duration;
    return loadedAnimation;
}

SkeletalAnimation SkeletalAnimationLoader::fromFile(
    const std::string& path, const std::map<std::string, std::uint32_t>& boneName2index) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 0);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading mesh " << importer.GetErrorString() << "\n";
        return SkeletalAnimation{ 0.0f };
    }
    if (scene->mNumAnimations == 0) {
        std::cerr << "file " << path << "does not contain animations\n";
        return SkeletalAnimation{ 0.0f };
    }
    return fromAssimpScene(scene, boneName2index);
}
