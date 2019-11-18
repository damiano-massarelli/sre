#ifndef PHONGMATERIAL_H
#define PHONGMATERIAL_H
#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"
#include <glm/common.hpp>
#include <memory>
#include "skeletalAnimation/SkeletralAnimationControllerComponent.h"

/**
 * Blinn-Phong Material.
 * Blinn-Phong materials provide a good-enough visual appearance
 * and are relatively efficient. However, PBRMaterial%s should be 
 * preferred for more realistic renderings.
 */
class BlinnPhongMaterial : public Material
{
private:
	std::int32_t mDiffuseColorLocation = -1;
	std::int32_t mSpecularColorLocation = -1;
	std::int32_t mShininessLocation = -1;
	std::int32_t mOpacityLocation = -1;
	std::int32_t mUseDiffuseMapLocation = -1;
	std::int32_t mUseSpecularMapLocation = -1;
	std::int32_t mBumpMapLocation = -1;
	std::int32_t mParallaxMapLocation = -1;
	std::int32_t mBonesLocation = -1;

    Texture diffuseMap;
    Texture specularMap;
	Texture bumpMap;
	Texture parallaxMap;

	bool mHasBumps = false;
	bool mHasParallax = false;

public:
    BlinnPhongMaterial(bool hasBumps = false, bool isAnimated = false, bool hasParallax = false);

	/**
	 * Sets the texture used for diffuse color.
	 * @param texture the texture used for diffuse color
	 */
    void setDiffuseMap(const Texture& texture);
	
	/**
	 * Sets the texture used for specular color.
	 * @param texture the texture used for specular color
	 */
    void setSpecularMap(const Texture& texture);
	
	/**
	 * Sets the texture used for bump mapping (aka normal mapping).
	 * In order to use a bump map, this material must be created using
	 * hasBumps = true (@see BlinnPhongMaterial::BlinnPhongMaterial()).
	 * Moreover, the corresponding mesh must be created with information
	 * about tangent space.
	 * @param texture the bump map
	 */
	void setBumpMap(const Texture& texture);

	/**
	 * Sets the texture used for parallax mapping.
	 * In order to use a parallax map this material must be
	 * created using hasParallax = true (@see BlinnPhongMaterial::BlinnPhongMaterial(), 
	 * @see BlinnPhongMaterialBuilder::setParallaxMap()).
	 * Moreover, the corresponding mesh must be created with information
	 * about tangent space.
	 * @param texture the height map
	 */
	void setParallaxMap(const Texture& texture);

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

	virtual std::size_t hash() const override;

	virtual bool equalsTo(const Material* rhs) const override;
};

using BlinnPhongMaterialPtr = std::shared_ptr<BlinnPhongMaterial>;

class BlinnPhongMaterialBuilder {
private:
    std::string mDiffuseMapPath;
    std::string mSpecularMapPath;

    Texture mDiffuseMap;
    Texture mSpecularMap;
	Texture mBumpMap;
	Texture mParallaxMap;

    bool mSpecularColorSet = false;
    glm::vec3 mSpecularColor{1.0f, 1.0f, 1.0f};

    bool mDiffuseColorSet = false;
    glm::vec3 mDiffuseColor{1.0f, 1.0f, 1.0f};

    float mShininess = 32;

	bool mAnimated = false;

public:
    BlinnPhongMaterialPtr build() {
        auto material = std::make_shared<BlinnPhongMaterial>(mBumpMap.isValid() , mAnimated, mParallaxMap.isValid());

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

		if (mBumpMap) 
			material->setBumpMap(mBumpMap);

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

	BlinnPhongMaterialBuilder& setBumpMap(const Texture& texture) {
		mBumpMap = texture;
		return *this;
	}

	BlinnPhongMaterialBuilder& setParallaxMap(const Texture& texture) {
		mParallaxMap = texture;
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
