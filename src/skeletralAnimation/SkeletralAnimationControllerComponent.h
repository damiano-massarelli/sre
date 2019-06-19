#pragma once
#include "Component.h"
#include "EventListener.h"
#include "EventListenerCrumb.h"
#include "Bone.h"
#include "SkeletralAnimation.h"
#include <vector>

class SkeletralAnimationControllerComponent :
	public Component, public EventListener
{
private:
	std::vector<Bone> mSkeleton;
	CrumbPtr mEventCrumb;

	std::vector<glm::mat4> getTransforms();

public:
	SkeletralAnimation animation;

	SkeletralAnimationControllerComponent(const GameObjectEH& go, const std::vector<Bone>& skeleton);

	virtual void onEvent(SDL_Event e) override;

	~SkeletralAnimationControllerComponent() = default;
};

