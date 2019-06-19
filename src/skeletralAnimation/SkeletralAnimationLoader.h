#pragma once

#include "SkeletralAnimation.h"
#include <assimp/scene.h>
#include <map>
#include <cstdint>

class SkeletralAnimationLoader
{
public:
	SkeletralAnimation fromAssimpScene(const aiScene* scene, const std::map<std::string, std::uint32_t>& boneName2index);

	SkeletralAnimationLoader();
	~SkeletralAnimationLoader();
};

