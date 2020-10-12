#include "rendering/materials/PBRMaterial.h"
#include "glm/gtc/epsilon.hpp"
#include "skeletalAnimation/SkeletalAnimationControllerComponent.h"
#include <functional>

template <typename T, typename... Rest> inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    if constexpr (sizeof...(rest) != 0) {
        hash_combine(seed, rest...);
    }
}

PBRMaterial::PBRMaterial(bool hasSkeletalAnimation)
    : Material{ { (hasSkeletalAnimation ? "shaders/animatedPhongVS.glsl" : "shaders/bumpedPhongVS.glsl") },
        { "shaders/pbrFS.glsl" } }
    , mHasSkeletalAnimation{ hasSkeletalAnimation } {
    unSupportedRenderPhases = (RenderPhase::FORWARD_RENDERING | RenderPhase::DEFERRED_RENDERING);

    ShaderScopedUsage useShader{ shader };

    mAlbedoLocation = shader.getLocationOf("material.albedo");
    mMetalnessLocation = shader.getLocationOf("material.metalness");
    mRoughnessLocation = shader.getLocationOf("material.roughness");
    mAOLocation = shader.getLocationOf("material.ao");
    mBonesLocation = shader.getLocationOf("bones",
        hasSkeletalAnimation);  // only used when animations are available

    mUseAlbedoMapLocation = shader.getLocationOf("material.useAlbedoMap");
    mUseNormalMapLocation = shader.getLocationOf("material.useNormalMap");
    mUseMetalnessMapLocation = shader.getLocationOf("material.useMetalnessMap");
    mUseRoughnessMapLocation = shader.getLocationOf("material.useRoughnessMap");
    mUseAOMapLocation = shader.getLocationOf("material.useAOMap");

    shader.setInt("material.albedoMap", 0);
    shader.setInt("material.normalMap", 1);
    shader.setInt("material.roughnessMap", 2);
    shader.setInt("material.metalnessMap", 3);
    shader.setInt("material.aoMap", 4);
}

void PBRMaterial::setAlbedoMap(const Texture& albedo) { mAlbedoMap = albedo; }

void PBRMaterial::setAlbedo(glm::vec3 albedo) { mAlbedo = albedo; }

void PBRMaterial::setNormalMap(const Texture& normal) { mNormalMap = normal; }

void PBRMaterial::useNormalMap(bool useNormalMap) { mUseNormalMap = useNormalMap; }

void PBRMaterial::setMetalnessMap(const Texture& metalness) { mMetalnessMap = metalness; }

void PBRMaterial::setUseMetalnessMap(bool useIt) { mUseMetalnessMap = useIt; }

void PBRMaterial::setMetalness(float metalness) { mMetalness = metalness; }

void PBRMaterial::setRoughnessMap(const Texture& roughness) { mRoughnessMap = roughness; }

void PBRMaterial::setUseRoughnessMap(bool useIt) { mUseRoughnessMap = useIt; }

void PBRMaterial::setRoughness(float roughness) { mRoughness = roughness; }

void PBRMaterial::setAmbientOcclusionMap(const Texture& ao) { mAOMap = ao; }

void PBRMaterial::setAmbientOcclusion(float ao) { mAO = ao; }

void PBRMaterial::setSkeletalAnimationController(
    std::shared_ptr<class SkeletalAnimationControllerComponent> controller) {
    mSkeletalAnimationController = controller;
}

void PBRMaterial::use() {
    shader.use();

    // update animation
    if (auto sac = mSkeletalAnimationController.lock()) {
        sac->updateBones(mBonesLocation, shader);
    }

    const bool useAlbedoMap = mUseAlbedoMap && mAlbedoMap.isValid();
    if (useAlbedoMap) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mAlbedoMap.getId());
    }
    shader.setInt(mUseAlbedoMapLocation, static_cast<int>(useAlbedoMap));
    shader.setVec3(mAlbedoLocation, mAlbedo);

    const bool useNormalMap = mUseNormalMap && mNormalMap.isValid();
    if (useNormalMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mNormalMap.getId());
    }
    shader.setInt(mUseNormalMapLocation, static_cast<int>(useNormalMap));

    const bool useRoughnessMap = mUseRoughnessMap && mRoughnessMap.isValid();
    if (useRoughnessMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mRoughnessMap.getId());
    }
    shader.setInt(mUseRoughnessMapLocation, static_cast<int>(useRoughnessMap));
    shader.setFloat(mRoughnessLocation, mRoughness);

    const bool useMetalnessMap = mUseMetalnessMap && mMetalnessMap.isValid();
    if (useMetalnessMap) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, mMetalnessMap.getId());
    }
    shader.setInt(mUseMetalnessMapLocation, static_cast<int>(useMetalnessMap));
    shader.setFloat(mMetalnessLocation, mMetalness);

    const bool useAOMap = mUseAOMap && mAOMap.isValid();
    if (useAOMap) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, mAOMap.getId());
    }
    shader.setInt(mUseAOMapLocation, static_cast<int>(useAOMap));
    shader.setFloat(mAOLocation, mAO);
}

void PBRMaterial::after() {
    for (int i = 4; i >= 0; --i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    shader.stop();
}

std::size_t PBRMaterial::hash() const {
    // Creating an int stacking 5 bits corresponding to each boolean
    // This is to avoid to put five other args in the hash_combine below
    const std::size_t boolMask = (mUseAlbedoMap << 0) | (mUseNormalMap << 1) | (mUseMetalnessMap << 2)
        | (mUseRoughnessMap << 3) | (mUseAOMap << 4) | (mHasSkeletalAnimation << 5);

    std::size_t seed = Material::hash();

    // floats and vec3 are left out as we want to have a threshold for them
    hash_combine(seed,
        mAlbedoMap.getId(),
        mNormalMap.getId(),
        mMetalnessMap.getId(),
        mRoughnessMap.getId(),
        mAOMap.getId(),
        boolMask);

    return seed;
}

bool PBRMaterial::equalsTo(const Material* rhs) const {
    // avoid dynamic cast. Same materials will have the same shader id
    // FIXME add type to Material
    if (shader.getId() != rhs->shader.getId())
        return false;

    constexpr float EPS = 0.001F;

    auto other = static_cast<const PBRMaterial*>(rhs);

    return Material::equalsTo(rhs) && glm::all(glm::epsilonEqual(mAlbedo, other->mAlbedo, EPS))
        && glm::epsilonEqual(mMetalness, other->mMetalness, EPS)
        && glm::epsilonEqual(mRoughness, other->mRoughness, EPS) && glm::epsilonEqual(mAO, other->mAO, EPS)
        && mAlbedoMap.getId() == other->mAlbedoMap.getId() && mNormalMap.getId() == other->mNormalMap.getId()
        && mMetalnessMap.getId() == other->mMetalnessMap.getId()
        && mRoughnessMap.getId() == other->mRoughnessMap.getId() && mAOMap.getId() == other->mAOMap.getId()
        && mUseAlbedoMap == other->mUseAlbedoMap && mUseNormalMap == other->mUseNormalMap
        && mUseMetalnessMap == other->mUseMetalnessMap && mUseRoughnessMap == other->mUseRoughnessMap
        && mUseAOMap == other->mUseAOMap && mHasSkeletalAnimation == other->mHasSkeletalAnimation;
}

PBRMaterial::~PBRMaterial() { }
