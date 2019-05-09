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

    shader.setVec3("cameraPosition", Engine::renderSys.camera->transform.getPosition());

    if (diffuseMap) {
        shader.setInt("material.useDiffuseMap", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap.id);
    } else
        shader.setInt("material.useDiffuseMap", 0);

    if (specularMap) {
        shader.setInt("material.useSpecularMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap.id);
    } else
        shader.setInt("material.useSpecularMap", 0);
}

PhongMaterial::~PhongMaterial()
{

}


