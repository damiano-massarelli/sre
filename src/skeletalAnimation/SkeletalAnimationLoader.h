#pragma once

#include "skeletalAnimation/SkeletalAnimation.h"
#include <assimp/scene.h>
#include <cstdint>
#include <map>

class SkeletalAnimationLoader {
public:
    SkeletalAnimationLoader() = default;

    /**
     * Load an animation from an already opened assimp scene.
     * @param scene the assimp scene containing the animation
     * @param boneName2index association between bone names and their indices in
     * skeleton (@see SkeletalAnimationControllerComponent)
     * @return a SkeletalAnimation. If no animation is contained in the scene
     * the SkeletalAnimation.getDuration will return 0
     */
    SkeletalAnimation fromAssimpScene(const aiScene* scene, const std::map<std::string, std::uint32_t>& boneName2index);

    /**
     * Load an animation from an already opened assimp scene.
     * @param path path to the file containing the animation
     * @param boneName2index association between bone names and their indices in
     * skeleton (@see SkeletalAnimationControllerComponent)
     * @return a SkeletalAnimation. If no animation is contained in the file the
     * SkeletalAnimation.getDuration will return 0
     */
    SkeletalAnimation fromFile(const std::string& path, const std::map<std::string, std::uint32_t>& boneName2index);
};
