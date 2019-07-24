#include "PointLight.h"
#include <algorithm>

PointLight::PointLight(const GameObjectEH& go)
	: Light{ go, Light::Type::POINT }
{

}

float PointLight::getRadius() const
{
	float linear = attenuationLinear;
	float constant = attenuationConstant;
	float quadratic = attenuationQuadratic;
	float lightMax = std::max({ diffuseColor.r, diffuseColor.g , diffuseColor.b,
		specularColor.r, specularColor.g, specularColor.b });
	float radius = (float)(-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 2.5) * lightMax))) / (2 * quadratic);

	return radius;
}

void PointLight::setCastShadowMode(ShadowCasterMode mode)
{
	throw std::logic_error("The method or operation is not implemented.");
}
