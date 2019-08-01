#include "PointShadowMaterial.h"
#include "Engine.h"

PointShadowMaterial::PointShadowMaterial()
	: Material{"shaders/pointShadowVS.glsl", "shaders/pointShadowGS.glsl", "shaders/pointShadowFS.glsl"}
{
	shader.use();
	mTransformLocation = shader.getLocationOf("transforms");
	mFarPlaneLocation = shader.getLocationOf("farPlane");
	mLightPositionLocation = shader.getLocationOf("lightPos");
}

void PointShadowMaterial::use()
{
	shader.use(); 
}

void PointShadowMaterial::setTransformations(const std::vector<glm::mat4>& transforms, float farPlane, const glm::vec3& lightPosition)
{
	shader.use();
	shader.setMat4Array(mTransformLocation, transforms);
	shader.setFloat(mFarPlaneLocation, farPlane);
	shader.setVec3(mLightPositionLocation, lightPosition);
}

