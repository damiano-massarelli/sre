#include "rendering/materials/PointShadowMaterial.h"
#include "Engine.h"

PointShadowMaterial::PointShadowMaterial()
    : Material{ "shaders/pointShadowVS.glsl", "shaders/pointShadowGS.glsl", "shaders/pointShadowFS.glsl" } {
    ShaderScopedUsage useShader{ shader };
    mTransformLocation = shader.getLocationOf("transforms");
    mFarPlaneLocation = shader.getLocationOf("farPlane");
    mLightPositionLocation = shader.getLocationOf("lightPos");
}

void PointShadowMaterial::use() { shader.use(); }

void PointShadowMaterial::after() { shader.stop(); }

void PointShadowMaterial::setTransformations(
    const std::vector<glm::mat4>& transforms, float farPlane, const glm::vec3& lightPosition) {
    ShaderScopedUsage useShader{ shader };
    shader.setMat4Array(mTransformLocation, transforms);
    shader.setFloat(mFarPlaneLocation, farPlane);
    shader.setVec3(mLightPositionLocation, lightPosition);
}
