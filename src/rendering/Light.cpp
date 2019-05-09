#include "Light.h"
#include <string>

Light::Light(const GameObjectEH& go, Light::Type lightType) : Component{go}, type{lightType}
{

}

Light::~Light()
{

}
