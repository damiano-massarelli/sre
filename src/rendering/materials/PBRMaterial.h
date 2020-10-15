#pragma once
#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"
#include <memory>

class PBRMaterial : public Material {
private:
    Texture mAlbedoMap;
    Texture mNormalMap;
    Texture mMetalnessMap;
    Texture mRoughnessMap;
    Texture mAOMap;

    glm::vec3 mAlbedo{ 1.f, 1.f, 1.f };
    float mMetalness{ 0.f };
    float mRoughness{ 1.f };
    float mAO{ 1.f };

    bool mUseAlbedoMap = true;
    bool mUseNormalMap = true;
    bool mUseMetalnessMap = true;
    bool mUseRoughnessMap = true;
    bool mUseAOMap = true;

    std::int32_t mAlbedoLocation;
    std::int32_t mMetalnessLocation;
    std::int32_t mRoughnessLocation;
    std::int32_t mAOLocation;

    std::int32_t mUseAlbedoMapLocation;
    std::int32_t mUseNormalMapLocation;
    std::int32_t mUseMetalnessMapLocation;
    std::int32_t mUseRoughnessMapLocation;
    std::int32_t mUseAOMapLocation;

    bool mHasSkeletalAnimation;
    std::int32_t mBonesLocation = -1;

    std::weak_ptr<class SkeletalAnimationControllerComponent> mSkeletalAnimationController;

public:
    PBRMaterial(bool hasSkeletalAnimation = false);

    // Setters
    void setAlbedoMap(const Texture& albedo);
    void setAlbedo(glm::vec3 albedo);

    void setNormalMap(const Texture& normal);
    void useNormalMap(bool useNormalMap);

    void setMetalnessMap(const Texture& metalness);
    void setUseMetalnessMap(bool useIt);
    void setMetalness(float metalness);

    void setRoughnessMap(const Texture& roughness);
    void setUseRoughnessMap(bool useIt);
    void setRoughness(float roughness);

    void setAmbientOcclusionMap(const Texture& ao);
    void setAmbientOcclusion(float ao);

    // Getters
    bool getUseAlbedoMap() const { return mUseAlbedoMap; }
    bool getUseNormalMap() const { return mUseNormalMap; }
    bool getUseMetalnessMap() const { return mUseMetalnessMap; }
    bool getUseRoughnessMap() const { return mUseRoughnessMap; }
    bool getUseAOMap() const { return mUseAOMap; }

    glm::vec3 getAlbedo() const { return mAlbedo; }
    float getMetalness() const { return mMetalness; }
    float getRoughness() const { return mRoughness; }
    float getAO() const { return mAO; }

    void setSkeletalAnimationController(std::shared_ptr<class SkeletalAnimationControllerComponent> controller);

    // Material base class implementation
    virtual void use() override;

    virtual void after() override;

    virtual std::size_t hash() const override;

    virtual bool equalsTo(const Material* rhs) const override;

    ~PBRMaterial();
};
