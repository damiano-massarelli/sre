#include "DirectionalLight.h"
#include "Engine.h"
#include "ShadowMappingSettings.h"

DirectionalLight::DirectionalLight(const GameObjectEH& go)
	: Light{ go, Light::Type::DIRECTIONAL }
{

}

const RenderTarget& DirectionalLight::getShadowMapTarget() const
{
	return mShadowMapTarget;
}

void DirectionalLight::setCastShadowMode(Light::ShadowCasterMode mode)
{
	Light::setCastShadowMode(mode);

	if (mode == Light::ShadowCasterMode::NO_SHADOWS)
		return;

	// destroys the old target if existed
	mShadowMapTarget = RenderTarget{};

	ShadowMappingSettings& settings = Engine::renderSys.shadowMappingSettings;
	mShadowMapTarget.create(settings.mapWidth, settings.mapHeight, false, true);
}
