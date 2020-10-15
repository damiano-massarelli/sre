#pragma once
#include "rendering/materials/Material.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

class PointShadowMaterial : public Material {
private:
    std::int32_t mTransformLocation;
    std::int32_t mFarPlaneLocation;
    std::int32_t mLightPositionLocation;

public:
    PointShadowMaterial();

    virtual void use() override;

    virtual void after() override;

    void setTransformations(const std::vector<glm::mat4>& transforms, float farPlane, const glm::vec3& lightPosition);

    ~PointShadowMaterial() = default;
};
