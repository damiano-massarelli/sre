#pragma once
#include "Material.h"
#include <vector>
#include <glm/glm.hpp>
#include <cstdint>

class PointShadowMaterial :
	public Material
{
private:
	std::int32_t mTransformLocation;
	std::int32_t mFarPlaneLocation;
	std::int32_t mLightPositionLocation;

public:
	PointShadowMaterial();

	virtual void use() override;

	void setTransformations(const std::vector<glm::mat4>& transforms, float farPlane, const glm::vec3& lightPosition);

	~PointShadowMaterial() = default;

};

