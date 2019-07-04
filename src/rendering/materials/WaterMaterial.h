#pragma once
#include "Material.h"
#include "RenderTarget.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"

class WaterMaterial :
	public Material, public EventListener
{
private:
	RenderTarget target;

	CrumbPtr mEventCrumb;

public:
	WaterMaterial();

	virtual void use() override;

	virtual void after() override;

	virtual void onEvent(SDL_Event e) override;

	~WaterMaterial() = default;
};

