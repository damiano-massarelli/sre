#include "BlinnPhongMaterial.h"
#include "Engine.h"

BlinnPhongMaterial::BlinnPhongMaterial() : Material{{"shaders/phongVS.glsl"},
                                          {"shaders/Light.glsl", "shaders/PhongLightCalculation.glsl", "shaders/phongFS.glsl"}}
{
    shader.bindUniformBlock("CommonMat", Engine::renderSys.COMMON_MAT_UNIFORM_BLOCK_INDEX);
    shader.bindUniformBlock("Lights", Engine::renderSys.LIGHT_UNIFORM_BLOCK_INDEX);
    shader.bindUniformBlock("Camera", Engine::renderSys.CAMERA_UNIFORM_BLOCK_INDEX);
}

void BlinnPhongMaterial::setDiffuseMap(const Texture& texture)
{
    diffuseMap = texture;

    diffuseMap.nameInShader = "material.diffuse";
    shader.use();
    shader.setInt(diffuseMap.nameInShader, 0);
}

void BlinnPhongMaterial::setSpecularMap(const Texture& texture)
{
    specularMap = texture;

    specularMap.nameInShader = "material.specular";
    shader.use();
    shader.setInt(specularMap.nameInShader, 1);
}

void BlinnPhongMaterial::use()
{
    shader.use();
    shader.setVec3("material.diffuseColor", diffuseColor);
    shader.setVec3("material.specularColor", specularColor);
    shader.setFloat("material.shininess", shininess);

    shader.setFloat("material.opacity", opacity);

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

    // disable backface culling
    if (isTwoSided) {
        glDepthMask(false);
        glDisable(GL_CULL_FACE);
    }
}

void BlinnPhongMaterial::after()
{
     // enable backface culling
    if (isTwoSided) {
        glDepthMask(true);
        glEnable(GL_CULL_FACE);
    }

    // unbind textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool BlinnPhongMaterial::needsOrderedRendering()
{
    return opacity < 1.0f;
}

float BlinnPhongMaterial::renderOrder(const glm::vec3& position)
{
    auto cam = Engine::renderSys.camera;
    if (cam)
        return glm::distance2(position, cam->transform.getPosition());
    else
        return position.z;
}

BlinnPhongMaterial::~BlinnPhongMaterial()
{

}



