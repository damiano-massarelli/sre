#pragma once
#include "Material.h"
#include "RenderTarget.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include "GameObjectEH.h"
#include "DeferredRenderingFBO.h"
#include "Texture.h"

class WaterMaterial :
	public Material, public EventListener
{
private:
	static constexpr float WAVE_SPEED = 0.3f;  

	RenderTarget mReflectionTarget;
	DeferredRenderingFBO mReflectionFbo;

	RenderTarget mRefractionTarget;
	DeferredRenderingFBO mRefractionFbo;

	GameObjectEH mReflectionCamera;

	CrumbPtr mEventCrumb;

	Texture mDuDvMap;
	std::int32_t mMoveDuDvLocation = 0;
	float mMoveDuDv = 0.0f;

	float mWaterY = 0.0f;

	void renderReflection();

	void renderRefraction();

public:
	WaterMaterial(float waterY, const Texture& dudvMap);

	virtual void use() override;

	virtual void after() override;

	virtual void onEvent(SDL_Event e) override;

	~WaterMaterial() = default;
};
