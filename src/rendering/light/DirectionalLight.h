#pragma once
#include "rendering/light/Light.h"
#include "rendering/RenderTarget.h"

class DirectionalLight
	: public Light
{
private:
	RenderTarget mShadowMapTarget;

public:
	DirectionalLight(const GameObjectEH& go);

	const RenderTarget& getShadowMapTarget() const;

	virtual void setCastShadowMode(Light::ShadowCasterMode mode) override;

	virtual ~DirectionalLight() = default;
};

