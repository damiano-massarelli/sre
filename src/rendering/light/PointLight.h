#pragma once
#include "Light.h"
#include "GameObjectEH.h"
#include "RenderTarget.h"

class PointLight :
	public Light
{
private:
	RenderTarget mPointShadowTarget;

public:
	PointLight(const GameObjectEH& go);

	float getRadius() const;

	virtual void setCastShadowMode(ShadowCasterMode mode) override;

	const RenderTarget& getPointShadowTarget() const;

	virtual ~PointLight() = default;
};

