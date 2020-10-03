#pragma once

#include "skeletalAnimation/Bone.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cstdint>

/**
 * Key frames for a bone.
 * Contains a list of key frames for the position, scale and rotation
 * of a bone.
 */
struct BoneKeyframes {
	/** position values */
	std::vector<glm::vec3> positions;

	/** scale values */
	std::vector<glm::vec3> scalings;

	/** rotations values */
	std::vector<glm::quat> rotations;


	/** position keys (indices correspond to positions) */
	std::vector<float> positionKeys;

	/** scaling keys (indices correspond to scalings) */
	std::vector<float> scalingKeys;

	/** rotation keys (indices correspond to rotations) */
	std::vector<float> rotationKeys;
};

/**
 * A skeletal animation.
 * An animation is composed of key frames for the bones of
 * a skeleton. These frames are stored in boneKeyFrames. The
 * i-th element in this vector contains the key frames for
 * the i-th bone in the skeleton (@see SkeletalAnimationComponent).
 */
class SkeletalAnimation
{
	friend class SkeletalAnimationLoader;

private:
	float mDuration = 0.0f;

	/**
	  * Key frames for bones. The i-th element in this vector
	  * contains key frames for the i-th bone of a skeleton.
	  */
	std::vector<BoneKeyframes> mBoneKeyFrames;

public:
	/** How an animation behaves once it reaches the end */
	enum class LoopDirection : std::int8_t {
		/** Stop when the animation finishes */
		STOP,
		/** Start over the animation once it is over */
		REPEAT,
		/** Repeat the animation going backwards */
		BOUNCE
	};

	/** how the animation behaves once it is over */
	LoopDirection loopDirection = LoopDirection::STOP;

	/** playback speed for this animation. 1.0f is the normal speed */
	float playbackSpeed = 1.f;

	SkeletalAnimation(float duration = 0.0f);

	/**
	 * @return the duration in seconds of this animation.
	 */
	float getDuration() const;

	/**
	 * @return whether this animation duration is 0.
	 */
	bool isEmpty() const;

	std::vector<glm::mat4> getAt(float ticks, const std::vector<Bone>& skeleton) const;
};

