#include "SkeletralAnimationControllerComponent.h"
#include "Engine.h"

SkeletralAnimationControllerComponent::SkeletralAnimationControllerComponent(const GameObjectEH& go, const std::vector<Bone>& skeleton,
	const std::map<std::string, std::uint32_t>& boneName2index)
	: Component{ go }, mSkeleton{ skeleton }, mBoneName2Index{ boneName2index }
{

}

void SkeletralAnimationControllerComponent::addAnimation(const std::string& name, const SkeletalAnimation& animation)
{
	mName2animation[name] = animation;
}

const SkeletalAnimation& SkeletralAnimationControllerComponent::getAnimation(const std::string& name) const
{
	auto anim = mName2animation.find(name);
	if (anim != mName2animation.end())
		return anim->second;

	return SkeletalAnimation{ 0.0f };
}

void SkeletralAnimationControllerComponent::playAnimation(const std::string& name)
{
	time.reset();
	mCurrentAnimation = name;
}

const std::map<std::string, std::uint32_t>& SkeletralAnimationControllerComponent::getBoneName2index() const
{
	return mBoneName2Index;
}

void SkeletralAnimationControllerComponent::updateBones(std::int32_t location, const Shader& shaderToUpdate)
{
	shaderToUpdate.setMat4Array(location, getTransforms());
}

std::vector<glm::mat4> SkeletralAnimationControllerComponent::getTransforms()
{
	const SkeletalAnimation& animation = mName2animation[mCurrentAnimation];

	std::vector<glm::mat4> transforms = animation.getAt(time.getSeconds(), mSkeleton);
	for (std::size_t i = 0; i < transforms.size(); ++i) {
		auto& bone = mSkeleton[i];
		if (bone.parent != -1)
			transforms[i] = (transforms[bone.parent] * transforms[i]); // parents are always before their children so it is ok
	}

	for (std::size_t i = 0; i < transforms.size(); ++i)
		transforms[i] *= mSkeleton[i].offset;

	return transforms;
}
