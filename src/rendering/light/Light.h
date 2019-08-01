#ifndef LIGHT_H
#define LIGHT_H
#include "Component.h"
#include "Shader.h"
#include "RenderTarget.h"
#include <glm/common.hpp>
#include <cstdint>
#include <memory>

/**
  * Light caster for the Blinn-Phong lighting model.
  * Each light caster can specify a type (Point Light, Directional Light
  * and Spot Light). Each light also provides attenuation factors. However,
  * directional lights won't take attenuation factors into account.
  * Directional and spot lights use a direction. This direction is given by
  * the z-axis orientation of the GameObject%'s Transform to which this Light is attached
  * @sa Transform.forward
  */
class Light : public Component
{
friend class RenderSystem;

public:
	/** Type of Light */
    enum class Type {
        POINT,
        DIRECTIONAL,
        SPOT
    };

	/** Type of shadow caster */
	enum class ShadowCasterMode {
		/** Does not cast shadows */
		NO_SHADOWS,
		/** Always cast shadows */
		DYNAMIC,
		/** Cast shadows only once and uses them in the following frame  */
		STATIC,
		/** Behaves as a static caster unless its position or orientation are changed */
		AUTO
	};

private:
	Type mType{ Type::POINT };

	ShadowCasterMode mShadowMode = ShadowCasterMode::NO_SHADOWS;
	bool mNeedsShadowUpdate = false;

	glm::vec3 mOldPosition{ 0.0f, 0.0f, 0.0f };
	glm::quat mOldRotation{ 0.0f, 0.0f, 0.0f, 1.0f };

protected:
	Light(const GameObjectEH& go, Type lightType = Type::POINT);

public:
    glm::vec3 ambientColor{0.1f};
    glm::vec3 diffuseColor{1.0f};
    glm::vec3 specularColor{1.0f};

    float attenuationConstant = 1.0f;
    float attenuationLinear = 0.045f;
    float attenuationQuadratic = 0.0075f;

    float innerAngle = 0.523599f; // 30 deg
    float outerAngle = 0.872665f; // 50 deg

	/** @return the type of Light. @see Light::Type */
	Type getType() const;

	/** @return the ShadowCasterMode for this Light */
	ShadowCasterMode getShadowCasterMode() const;

	/**
	 * Sets the ShadowCasterMode for this Light.
	 * @param mode the mode to use
	 */
	virtual void setCastShadowMode(ShadowCasterMode mode);

	/** Notifies that the shadows casted by this Light should be updated.
	 * This has not effect if ShadowCasterMode is ShadowCasterMode::NO_SHADOW
	 * and it is useless if ShadowCasterMode is ShadowCasterMode::DYNAMIC */
	void updateShadow();

	bool needsShadowUpdate();

    virtual ~Light();
};

using LightPtr = std::shared_ptr<Light>;

#endif // LIGHT_H
