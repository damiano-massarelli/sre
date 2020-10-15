#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

struct Bone {
    Bone() = default;

    std::int32_t parent = -1;
    std::string name{};

    /** Coverts from local space to bone space */
    glm::mat4 offset{};

    glm::vec3 position{};
    glm::quat rotation{};
    glm::vec3 scale{};
};
