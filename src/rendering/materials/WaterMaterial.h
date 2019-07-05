#pragma once
#include "Material.h"
#include "RenderTarget.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include "GameObjectEH.h"

class WaterMaterial :
	public Material, public EventListener
{
private:
	RenderTarget target;

	CrumbPtr mEventCrumb;

	float mWaterY = 0.0f;

public:
	WaterMaterial(float waterY);

	virtual void use() override;

	virtual void after() override;

	virtual void onEvent(SDL_Event e) override;

	~WaterMaterial() = default;
};
