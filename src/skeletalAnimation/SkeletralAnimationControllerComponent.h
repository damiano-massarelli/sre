#pragma once
#include "components/Component.h"
#include "skeletalAnimation/Bone.h"
#include "timer/Timer.h"
#include "skeletalAnimation/SkeletalAnimation.h"
#include <vector>
#include <map>
#include <cstdint>

/**
 * Controls skeletal animations for a GameObject.
 * Skeletal animations are instances of SkeletalAnimation
 * and can be loaded from file using SkeletalAnimationLoader.
 * To add an animation use addAnimation(). Each animation
 * has its own name so that it can be played using
 * playAnimation().
 * To control the execution of a single animation use the 
 * time instance variable (@see Timer).
 * If a GameObject imported using GameObjectLoader has 
 * an animation, it will be automatically added to this component
 * and its name will be "default".
 */
class SkeletralAnimationControllerComponent :
	public Component
{
private:
	std::vector<Bone> mSkeleton;
	std::map<std::string, std::uint32_t> mBoneName2Index;

	std::map<std::string, SkeletalAnimation> mName2animation;

	std::string mCurrentAnimation;

	std::vector<glm::mat4> getTransforms();

public:
	/** Current time of animation */
	Timer time;


	SkeletralAnimationControllerComponent(const GameObjectEH& go, const std::vector<Bone>& skeleton,
		const std::map<std::string, std::uint32_t>& boneName2index);

	/**
	 * Adds a SkeletalAnimation to this component.
	 * @param name the name of the SkeletalAnimation (should be unique)
	 * @param animation the SkeletalAnimation to add
	 */
	void addAnimation(const std::string& name, const SkeletalAnimation& animation);

	/**
	 * Gets the animation with the given name.
	 * @param name the name of the animation
	 * @return a pointer to the searched animation, nullptr if no animation is found.
	 * CAVEAT: this pointer may become invalid if new animations are added.
	 */
	SkeletalAnimation* getAnimation(const std::string& name);

	/**
	 * Plays a SkeletalAnimation.
	 * The name of the SkeletalAnimation is decided when it is 
	 * added to this component using addAnimation.
	 * @param name the name of the animation to play
	 */
	void playAnimation(const std::string& name);

	/**
	 * Mapping among external and internal bone representation.
	 * Bones stored in files have names. The internal representation of 
	 * bones uses ids instead.
	 * @return the mapping among bone names and bones ids
	 * @see SkeletalAnimationLoader::fromFile
	 */
	const std::map<std::string, std::uint32_t>& getBoneName2index() const;

	/**
	 * Updates bones' transformation matrices in the shader.
	 * Materials that support animations should call this method passing
	 * their shader and the location of the array containing bones' transformation
	 * matrices. This method will automatically update that array.
	 * CAVEAT: shaderToUpdate must be in use (@see Shader::use) when this
	 * method is called.
	 * @param location the location of the bones' transformation matrices array
	 * @param shaderToUpdate the shader used by the material
	 */
	void updateBones(std::int32_t location, const Shader& shaderToUpdate);
};

