#ifndef LIGHT_H
#define LIGHT_H
#include "Component.h"
#include "Shader.h"
#include <glm/common.hpp>
#include <cstdint>
#include <memory>

/**
  * Light caster for the Blinn-Phong lighting model.
  * Each light caster can specify a type (Point Light, Directional Light
  * and Spot Light). Each light also provides attenuation factors. However,
  * directional lights won't take attenuation factors into account
  * Directional and spot lights use a direction. This direction is given by
  * the z-axis orientation of the game object to which this light is attached
  * @sa Transform.forward
  */
class Light : public Component
{
    friend class RenderSystem;

    public:
        enum class Type {
            POINT,
            DIRECTIONAL,
            SPOT
        };

    public:
        Type type{Type::POINT};
        glm::vec3 ambientColor{0.1f};
        glm::vec3 diffuseColor{1.0f};
        glm::vec3 specularColor{1.0f};

        float attenuationConstant = 1.0f;
        float attenuationLinear = 0.045f;
        float attenuationQuadratic = 0.0075f;

        float innerAngle = 0.523599f; // 30 deg
        float outerAngle = 0.872665f; // 50 deg

		bool castShadow = false;

        Light(const GameObjectEH& go, Type lightType = Type::POINT);

        virtual ~Light();
};

using LightPtr = std::shared_ptr<Light>;

#endif // LIGHT_H
