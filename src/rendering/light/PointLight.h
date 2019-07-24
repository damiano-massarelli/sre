#pragma once
#include "Light.h"
#include "GameObjectEH.h"

class PointLight :
	public Light
{
public:
	PointLight(const GameObjectEH& go);

	float getRadius() const;

	virtual void setCastShadowMode(ShadowCasterMode mode) override;

	virtual ~PointLight() = default;
};

