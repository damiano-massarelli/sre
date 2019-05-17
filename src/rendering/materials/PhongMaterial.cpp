#include "PhongMaterial.h"
#include "Engine.h"

PhongMaterial::PhongMaterial() : Material{{"shaders/phongVS.glsl"},
                                          {"shaders/Light.glsl", "shaders/PhongLightCalculation.glsl", "shaders/phongFS.glsl"}}
{
    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
    shader.bindUniformBlock("Lights", Engine::renderSys.LIGHT_UNIFORM_BLOCK_INDEX);
}

void PhongMaterial::setDiffuseMap(const Texture& texture)
{
    diffuseMap = texture;

    diffuseMap.nameInShader = "material.diffuse";
    shader.use();
    shader.setInt(diffuseMap.nameInShader, 0);
}

void PhongMaterial::setSpecularMap(const Texture& texture)
{
    specularMap = texture;

    specularMap.nameInShader = "material.specular";
    shader.use();
    shader.setInt(specularMap.nameInShader, 1);
}

void PhongMaterial::use()
{
    shader.use();
    shader.setVec3("material.diffuseColor", diffuseColor);
    shader.setVec3("material.specularColor", specularColor);
    shader.setFloat("material.shininess", shininess);

    shader.setFloat("material.opacity", opacity);

    shader.setVec3("cameraPosition", Engine::renderSys.camera->transform.getPosition());

    if (diffuseMap) {
        shader.setInt("material.useDiffuseMap", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap.getId());
    } else
        shader.setInt("material.useDiffuseMap", 0);

    if (specularMap) {
        shader.setInt("material.useSpecularMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap.getId());
    } else
        shader.setInt("material.useSpecularMap", 0);
}

void PhongMaterial::after()
{
    // unbind textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool PhongMaterial::needsOrderedRendering()
{
    return opacity < 1.0f;
}

float PhongMaterial::renderOrder(const glm::vec3& position)
{
    auto cam = Engine::renderSys.camera;
    if (cam)
        return glm::distance2(position, cam->transform.getPosition());
    else
        return position.z;
}

PhongMaterial::~PhongMaterial()
{

}



