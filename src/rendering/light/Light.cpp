#include "Light.h"
#include "Engine.h"
#include "RenderSystem.h"

Light::Light(const GameObjectEH& go, Light::Type lightType) : Component{ go }, mType{ lightType }
{

}

Light::Type Light::getType() const
{
	return mType;
}

Light::ShadowCasterMode Light::getShadowCasterMode() const
{
	return mShadowMode;
}

void Light::setCastShadowMode(ShadowCasterMode mode)
{
	if (mode == ShadowCasterMode::STATIC)
		mNeedsShadowUpdate = true;

	if (mode == ShadowCasterMode::AUTO) {
		mNeedsShadowUpdate = true;
		mOldPosition = gameObject->transform.getPosition();
		mOldRotation = gameObject->transform.getRotation();
	}

	mShadowMode = mode;
}

void Light::updateShadow()
{
	mNeedsShadowUpdate = true;
}

bool Light::needsShadowUpdate()
{
	if (mNeedsShadowUpdate) {
		mNeedsShadowUpdate = false;
		return true;
	}

	if (mShadowMode == ShadowCasterMode::DYNAMIC) return true;

	if (mShadowMode == ShadowCasterMode::AUTO) {
		const Transform& transform = gameObject->transform;
		const glm::vec3& pos = transform.getPosition();
		const glm::quat& rot = transform.getRotation();

		bool shouldUpdate = glm::any(glm::notEqual(pos, mOldPosition))
			|| glm::any(glm::notEqual(rot, mOldRotation));

		mOldPosition = pos;
		mOldRotation = rot;

		return shouldUpdate;
	}

	return false;
}

Light::~Light()
{

}
