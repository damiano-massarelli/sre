#include "rendering/materials/BlinnPhongMaterial.h"
#include "Engine.h"
#include <iostream>
#include <vector>
#include <functional>
#include <glm/gtx/hash.hpp>

std::vector<std::string> getVertexShaders(bool hasBumps, bool isAnimated, bool hasParallax) {
	std::vector<std::string> shaders;
	// animated and with bumps
	if (isAnimated)							shaders.push_back("shaders/animatedPhongVS.glsl");
	else if (hasParallax)					shaders.push_back("shaders/parallaxPhongVS.glsl");
	else if (hasBumps)						shaders.push_back("shaders/bumpedPhongVS.glsl");
	else									shaders.push_back("shaders/phongVS.glsl");
	return shaders;
}

std::vector<std::string> getFragmentShaders(bool hasBumps, bool hasParallax) {
	if (hasParallax) return { "shaders/parallaxPhongFS.glsl" };
	if (hasBumps) return { "shaders/bumpedPhongFS.glsl" };

	else return { "shaders/phongFS.glsl" };
}

BlinnPhongMaterial::BlinnPhongMaterial(bool hasBumps, bool isAnimated, bool hasParallax)
	: Material{getVertexShaders(hasBumps, isAnimated, hasParallax),
			   {},
               getFragmentShaders(hasBumps, hasParallax)},
	 mHasBumps{hasBumps}, mHasParallax{hasParallax}
{
	unSupportedRenderPhases |= (RenderPhase::FORWARD_RENDERING | RenderPhase::PBR);

	shader.use();

    shader.bindUniformBlock("CommonMat", RenderSystem::COMMON_MAT_UNIFORM_BLOCK_INDEX);
	if (hasParallax) // parallax mapping needs the position of the camera
		shader.bindUniformBlock("Camera", RenderSystem::CAMERA_UNIFORM_BLOCK_INDEX);

	mDiffuseColorLocation		= shader.getLocationOf("material.diffuseColor");
	mSpecularColorLocation		= shader.getLocationOf("material.specularColor");
	mShininessLocation			= shader.getLocationOf("material.shininess");
	mOpacityLocation			= shader.getLocationOf("material.opacity");
	mUseDiffuseMapLocation		= shader.getLocationOf("material.useDiffuseMap");
	mUseSpecularMapLocation		= shader.getLocationOf("material.useSpecularMap");
	mBumpMapLocation			= shader.getLocationOf("material.bump", hasBumps); // only used when has bumps is true
	mParallaxMapLocation		= shader.getLocationOf("material.parallax", hasParallax); // only used when has parallax is true
	mBonesLocation				= shader.getLocationOf("bones", isAnimated); // only used when animations are available
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

void BlinnPhongMaterial::setBumpMap(const Texture& texture)
{
	if (!mHasBumps) {
		std::cerr << "setting bump map on a non-bumped material, discarded.\n";
		return;
	}
	bumpMap = texture;
	shader.use();
	shader.setInt(mBumpMapLocation, 2);
}

void BlinnPhongMaterial::setParallaxMap(const Texture& texture)
{
	if (!mHasParallax) {
		std::cerr << "setting parallax map on a material that does not support it, discarded. \n";
		return;
	}
	parallaxMap = texture;
	shader.use();
	shader.setInt(mParallaxMapLocation, 3);
}

void BlinnPhongMaterial::use()
{
    shader.use();

    shader.setVec3(mDiffuseColorLocation, diffuseColor);
    shader.setVec3(mSpecularColorLocation, specularColor);
    shader.setFloat(mShininessLocation, shininess);

    shader.setFloat(mOpacityLocation, opacity);

	if (auto sac = skeletalAnimationController.lock()) {
		sac->updateBones(mBonesLocation, shader);
	}

    if (diffuseMap) {
        shader.setInt(mUseDiffuseMapLocation, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap.getId());
    } else
        shader.setInt(mUseDiffuseMapLocation, 0);

    if (specularMap) {
        shader.setInt(mUseSpecularMapLocation, 2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap.getId());
    } else
        shader.setInt(mUseSpecularMapLocation, 0);

	if (mHasBumps && bumpMap) {
		shader.setInt(mBumpMapLocation, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, bumpMap.getId());
	}

	if (mHasParallax && parallaxMap) {
		shader.setInt(mParallaxMapLocation, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, parallaxMap.getId());
	}

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
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

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
    auto cam = Engine::renderSys.getCamera();
    if (cam)
        return glm::distance2(position, cam->transform.getPosition());
    else
        return position.z;
}

std::size_t BlinnPhongMaterial::hash() const
{
	// this hash is far from being perfect but it is fast
	// since it has to be computed every frame. Some collisions are
	// allowed
	return Material::hash()
		+ diffuseMap.getId()
		+ specularMap.getId()
		+ bumpMap.getId()
		+ parallaxMap.getId();
}

bool BlinnPhongMaterial::equalsTo(const Material* rhs) const
{
	// avoid dynamic cast. Same materials will have the same shader id
	// FIXME add type to Material
	if (shader.getId() != rhs->shader.getId()) return false;

	auto other = static_cast<const BlinnPhongMaterial*>(rhs);

	return Material::equalsTo(rhs)
		&& diffuseMap.getId() == other->diffuseMap.getId()
		&& specularMap.getId() == other->specularMap.getId()
		&& bumpMap.getId() == other->bumpMap.getId()
		&& parallaxMap.getId() == other->parallaxMap.getId()
		&& shininess == other->shininess
		&& diffuseColor == other->diffuseColor
		&& specularColor == other->specularColor
		&& opacity == other->opacity;
}

BlinnPhongMaterial::~BlinnPhongMaterial()
{

}
