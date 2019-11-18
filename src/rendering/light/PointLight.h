#pragma once
#include "rendering/light/Light.h"
#include "gameobject/GameObjectEH.h"
#include "rendering/RenderTarget.h"

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

