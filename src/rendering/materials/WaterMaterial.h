#pragma once
#include "events/EventListener.h"
#include "events/EventListenerCrumb.h"
#include "gameobject/GameObjectEH.h"
#include "rendering/RenderTarget.h"
#include "rendering/deferredRendering/DeferredRenderingFBO.h"
#include "rendering/materials/Material.h"
#include "rendering/materials/Texture.h"
#include "cameras/CameraComponent.h"
#include <memory>

/**
 * Material for water rendering.
 * This material is used to render water usually
 * on a plane (@see MeshCreator::plane).
 * This material supposes that the plane is not tiled and
 * that its normal is pointing upwards.
 * A dudvMap Texture is used to create the wave effect
 * and a normalMap Texture can be use to align specular
 * reflections with the waves.
 *
 * Based on ThinMatrix tutorial.
 */
class WaterMaterial : public Material, public EventListener {
private:
    Texture mReflectionTexture;
    RenderTarget mReflectionTarget;
    GBuffer mReflectionGBuffer;

    Texture mRefractionTexture;
    RenderTarget mRefractionTarget;
    GBuffer mRefractionGBuffer;

    std::shared_ptr<CameraComponent> mReflectionCameraComponent;
    GameObjectEH mReflectionCamera;

    CrumbPtr mEventCrumb;

    Texture mDuDvMap;
    std::int32_t mMoveDuDvLocation = 0;
    float mMoveDuDv = 0.0f;

    Texture mNormalMap;

    float mWaterY = 0.0f;

    void renderReflection();

    void renderRefraction();

public:
    /** Speed of the wave movement */
    float waveSpeed = 0.05f;

    /**
     * Creates a new WaterMaterial.
     * @param waterY the height of the water (should be the same as the y
     * component of Transform::getPosition)
     * @param dudvMap Texuture used to create the wave effect
     * @param normalMap Texture used to create specular reflections on waves
     */
    WaterMaterial(float waterY, const Texture& dudvMap, const Texture& normalMap);

    virtual void use() override;

    virtual void after() override;

    virtual void onEvent(SDL_Event e) override;

    virtual std::size_t hash() const override;

    virtual bool equalsTo(const Material* rhs) const override;

    ~WaterMaterial() = default;
};
