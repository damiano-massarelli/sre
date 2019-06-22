#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H
#include "Material.h"
#include "Texture.h"
#include <glm/common.hpp>
#include <memory>
#include <SkeletralAnimationControllerComponent.h>

class BlinnPhongMaterial : public Material
{
private:
	std::int32_t mDiffuseColorLocation = -1;
	std::int32_t mSpecularColorLocation = -1;
	std::int32_t mShininessLocation = -1;
	std::int32_t mOpacityLocation = -1;
	std::int32_t mUseDiffuseMapLocation = -1;
	std::int32_t mUseSpecularMapLocation = -1;
	std::int32_t mBonesLocation = -1;

    Texture diffuseMap;
    Texture specularMap;

public:
    BlinnPhongMaterial(bool isAnimated = false);

	/** The texture used for diffuse color */
    void setDiffuseMap(const Texture& texture);
	/** The texture used for specular color */
    void setSpecularMap(const Texture& texture);

	/** The diffuse color, if a diffuse map is set it is multiplied by this color */
    glm::vec3 diffuseColor{1.0f, 1.0f, 1.0f};
	/** The specular color, if a specular map is set it is multiplied by this color */
    glm::vec3 specularColor{1.0f, 1.0f, 1.0f};
	/** shininess according to the Blinn-Phong lighting model */
    float shininess = 32.0f;
	/** 0.0 means transparent, 1.0 is fully opaque */
    float opacity = 1.0f;

	/* Shaders are cached among different objects, if two objects were to share the same shader
	they would end up using the same animation. Hence before being used, each material ask its
	animation controller to update the bone transformation matrices */
	/** animation controller used for skeletal animation */
	std::weak_ptr<SkeletralAnimationControllerComponent> skeletalAnimationController;

    virtual void use() override;

    virtual void after() override;

    virtual bool needsOrderedRendering() override;

    virtual float renderOrder(const glm::vec3& position) override;

    virtual ~BlinnPhongMaterial();
};

using BlinnPhongMaterialPtr = std::shared_ptr<BlinnPhongMaterial>;

class BlinnPhongMaterialBuilder {
private:
    std::string mDiffuseMapPath;
    std::string mSpecularMapPath;

    Texture mDiffuseMap;
    Texture mSpecularMap;

    bool mSpecularColorSet = false;
    glm::vec3 mSpecularColor{1.0f, 1.0f, 1.0f};

    bool mDiffuseColorSet = false;
    glm::vec3 mDiffuseColor{1.0f, 1.0f, 1.0f};

    float mShininess = 32;

	bool mAnimated = false;

public:
    BlinnPhongMaterialPtr build() {
        auto material = std::make_shared<BlinnPhongMaterial>(mAnimated);

        if (mDiffuseMapPath != "")
            material->setDiffuseMap(Texture::loadFromFile(mDiffuseMapPath));

        if (mDiffuseMap) {
            material->setDiffuseMap(mDiffuseMap);
            mDiffuseColor = glm::vec3{1.0f, 1.0f, 1.0f};
        }

        if (mSpecularMapPath != "") {
            material->setSpecularMap(Texture::loadFromFile(mSpecularMapPath));
        }

        if (mSpecularMap) {
            material->setSpecularMap(mSpecularMap);
            mSpecularColor = glm::vec3{1.0f, 1.0f, 1.0f};
        }

        material->diffuseColor = mDiffuseColor;
        material->specularColor = mSpecularColor;
        material->shininess = mShininess;

        return material;
    }

    BlinnPhongMaterialBuilder& setDiffuseMap(const std::string& path) {
        mDiffuseMapPath = path;
        return *this;
    }

    BlinnPhongMaterialBuilder& setDiffuseMap(const Texture& texture) {
        mDiffuseMap = texture;
        return *this;
    }

    BlinnPhongMaterialBuilder& setSpecularMap(const std::string& path) {
        mSpecularMapPath = path;
        return *this;
    }

    BlinnPhongMaterialBuilder& setSpecularMap(const Texture& texture) {
        mSpecularMap = texture;
        return *this;
    }

    BlinnPhongMaterialBuilder& setShininess(float s) {
        mShininess = s;
        return *this;
    }

    BlinnPhongMaterialBuilder& setDiffuseColor(const glm::vec3& color) {
        mDiffuseColor = color;
        mDiffuseColorSet = true;
        return *this;
    }

    BlinnPhongMaterialBuilder& setSpecularColor(const glm::vec3& color) {
        mSpecularColor = color;
        mSpecularColorSet = true;
        return *this;
    }

	BlinnPhongMaterialBuilder& setAnimated(bool animated) {
		mAnimated = animated;
		return *this;
	}
};

#endif // PHONGMATERIAL_H
