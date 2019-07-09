#pragma once
#include "Material.h"
#include "RenderTarget.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include "GameObjectEH.h"
#include "DeferredRenderingFBO.h"
#include "Texture.h"

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
class WaterMaterial :
	public Material, public EventListener
{
private:
	RenderTarget mReflectionTarget;
	DeferredRenderingFBO mReflectionFbo;

	RenderTarget mRefractionTarget;
	DeferredRenderingFBO mRefractionFbo;

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
	 * @param waterY the height of the water (should be the same as the y component of Transform::getPosition)
	 * @param dudvMap Texuture used to create the wave effect
	 * @param normalMap Texture used to create specular reflections on waves
	 */
	WaterMaterial(float waterY, const Texture& dudvMap, const Texture& normalMap);

	virtual void use() override;

	virtual void after() override;

	virtual void onEvent(SDL_Event e) override;

	~WaterMaterial() = default;
};
