#pragma once
#include "components/Component.h"
#include "skeletalAnimation/Bone.h"
#include "skeletalAnimation/SkeletalAnimation.h"
#include "timer/Timer.h"
#include <cstdint>
#include <map>
#include <vector>

/**
 * Controls skeletal animations for a GameObject.
 * Skeletal animations are instances of SkeletalAnimation
 * and can be loaded from file using SkeletalAnimationLoader.
 * To add an animation use addAnimation(). Each animation
 * has its own name so that it can be used using
 * setCurrentAnimation().
 * To control the execution of the current animation use
 * playCurrentAnimation(), pauseCurrentAnimation() and stopCurrentAnimation().
 * If a GameObject imported using GameObjectLoader has
 * an animation, it will be automatically added to this component
 * and its name will be "default".
 */
class SkeletalAnimationControllerComponent : public Component {
private:
    std::vector<Bone> mSkeleton;
    std::map<std::string, std::uint32_t> mBoneName2Index;

    std::map<std::string, SkeletalAnimation> mName2animation;

    std::string mCurrentAnimation;

    std::vector<glm::mat4> getTransforms();

    /** Current time of animation */
    Timer mTime;

public:
    SkeletalAnimationControllerComponent(const GameObjectEH& go,
        const std::vector<Bone>& skeleton,
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
     * @return a pointer to the searched animation, nullptr if no animation is
     * found. CAVEAT: this pointer may become invalid if new animations are
     * added.
     */
    const SkeletalAnimation* getAnimation(const std::string& name) const;

    /**
     * Gets the animation with the given name.
     * @param name the name of the animation
     * @return a pointer to the searched animation, nullptr if no animation is
     * found. CAVEAT: this pointer may become invalid if new animations are
     * added.
     */
    SkeletalAnimation* getAnimation(const std::string& name);

    /**
     * Sets the current animation.
     * The animation won't be played. Use setAndPlayAnimation or
     * playCurrentAnimation
     * @param name the name of the animation to set.
     * @return true if the animation exists, false otherwise.
     */
    bool setCurrentAnimation(const std::string& name);

    /**
     * Sets the current animation and plays it
     * @param name the name of the animation.
     * @return true if the animation exists, false otherwise.
     */
    bool setCurrentAnimationAndPlay(const std::string& name);

    /**
     * Plays or resumes the current animation.
     * @return true if the animation was stopped or paused, false otherwise
     * (even if no current animation is set).
     */
    bool playCurrentAnimation();

    /**
     * Pauses the current animation.
     * @return true if the animation playing, false otherwise (even if no
     * current animation is set).
     */
    bool pauseCurrentAnimation();

    /**
     * Stops the current animation.
     * @return true if the animation playing or paused, false otherwise (even if
     * no current animation is set).
     */
    bool stopCurrentAnimation();

    /**
     * @return whether the current animation is playing. false if no current
     * animation is set.
     */
    bool isCurrentAnimationPlaying() const;

    /**
     * @return whether the current animation is paused. false if no current
     * animation is set.
     */
    bool isCurrentAnimationPaused() const;

    /**
     * @return whether the current animation is stopped. false if no current
     * animation is set.
     */
    bool isCurrentAnimationStopped() const;

    /**
     * Gets the time of the current animation in seconds.
     * The time also depends on the value of SkeletalAnimation::loopDirection.
     * @return the time of the current animation, 0.f if no animation is set or
     * it is stopped.
     */
    float getCurrentAnimationTime() const;

    /**
     * Sets the time of the current animation.
     * If the current animation was stopped (@see isCurrentAnimationStopped) the
     * animation will be started an paused.
     * @param time the time to set.
     * @return true if a current animation was set, false otherwise.
     */
    void setCurrentAnimationTime(float time);

    /**
     * @return the name of the current animation
     */
    const std::string& getCurrentAniamtionName() const { return mCurrentAnimation; }

    /**
     * @return the current animation, the one being played.
     * CAVEAT: this pointer may become invalid if new animations are added.
     */
    const SkeletalAnimation* getCurrentAniamtion() const;

    /**
     * @return the current animation, the one being played.
     * CAVEAT: this pointer may become invalid if new animations are added.
     */
    SkeletalAnimation* getCurrentAniamtion();

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
     * their shader and the location of the array containing bones'
     * transformation matrices. This method will automatically update that
     * array. CAVEAT: shaderToUpdate must be in use (@see Shader::use) when this
     * method is called.
     * @param location the location of the bones' transformation matrices array
     * @param shaderToUpdate the shader used by the material
     */
    void updateBones(std::int32_t location, const Shader& shaderToUpdate);
};
