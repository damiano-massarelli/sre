#include "SkeletralAnimationControllerComponent.h"
#include "Engine.h"

float elapsed = 0.0f;

std::vector<glm::mat4> SkeletralAnimationControllerComponent::getTransforms()
{
	std::vector<glm::mat4> transforms = animation.getAt(elapsed, mSkeleton);
	elapsed += 0.01f;
	for (std::size_t i = 0; i < transforms.size(); ++i) {
		auto& bone = mSkeleton[i];
		if (bone.parent != -1)
			transforms[i] = (transforms[bone.parent] * transforms[i]); // parents are always before their children so it is ok
	}

	for (auto i = 0; i < transforms.size(); ++i)
		transforms[i] *= mSkeleton[i].offset;

	return transforms;
}

SkeletralAnimationControllerComponent::SkeletralAnimationControllerComponent(const GameObjectEH& go, const std::vector<Bone>& skeleton)
	: Component{ go }, mSkeleton{ skeleton }
{
	mEventCrumb = Engine::eventManager.addListenerFor(EventManager::ENTER_FRAME_EVENT, this, true);
}

#include <glm/gtx/string_cast.hpp>
#include <iostream>

void SkeletralAnimationControllerComponent::onEvent(SDL_Event e)
{
	auto t = getTransforms();
	auto mat = gameObject->transform.getChildren()[0]->getMaterials()[0];
	mat->use();
	mat->shader.setMat4Array("bones", t);
}
