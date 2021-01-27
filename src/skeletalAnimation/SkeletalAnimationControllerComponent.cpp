#include "skeletalAnimation/SkeletalAnimationControllerComponent.h"

SkeletalAnimationControllerComponent::SkeletalAnimationControllerComponent(const GameObjectEH& go,
    const std::vector<Bone>& skeleton,
    const std::map<std::string, std::uint32_t>& boneName2index)
    : Component{ go }
    , mSkeleton{ skeleton }
    , mBoneName2Index{ boneName2index } { }

void SkeletalAnimationControllerComponent::addAnimation(const std::string& name, const SkeletalAnimation& animation) {
    mName2animation[name] = animation;
}

const SkeletalAnimation* SkeletalAnimationControllerComponent::getAnimation(const std::string& name) const {
    const SkeletalAnimation* animation = nullptr;
    auto anim = mName2animation.find(name);
    if (anim != mName2animation.end())
        animation = &anim->second;

    return animation;
}

SkeletalAnimation* SkeletalAnimationControllerComponent::getAnimation(const std::string& name) {
    SkeletalAnimation* animation = nullptr;
    auto anim = mName2animation.find(name);
    if (anim != mName2animation.end())
        animation = &anim->second;

    return animation;
}

bool SkeletalAnimationControllerComponent::setCurrentAnimation(const std::string& name) {
    if (mName2animation.find(name) != mName2animation.end()) {
        mTime.stop();
        mTime.setTimeSpeedMultiplier(1.f);
        mCurrentAnimation = name;
        return true;
    }

    return false;
}

bool SkeletalAnimationControllerComponent::setCurrentAnimationAndPlay(const std::string& name) {
    const bool wasSet = setCurrentAnimation(name);
    if (wasSet) {
        playCurrentAnimation();
    }

    return wasSet;
}

const SkeletalAnimation* SkeletalAnimationControllerComponent::getCurrentAnimation() const {
    return getAnimation(mCurrentAnimation);
}

SkeletalAnimation* SkeletalAnimationControllerComponent::getCurrentAnimation() {
    return getAnimation(mCurrentAnimation);
}

const std::map<std::string, std::uint32_t>& SkeletalAnimationControllerComponent::getBoneName2index() const {
    return mBoneName2Index;
}

void SkeletalAnimationControllerComponent::updateBones(std::int32_t location, const Shader& shaderToUpdate) {
    const SkeletalAnimation* currentAnimation = getCurrentAnimation();
    if (currentAnimation != nullptr) {

        // Update playback speed if it changed in the animation
        if (currentAnimation->playbackSpeed != mTime.getTimeSpeedMultiplier()) {
            mTime.setTimeSpeedMultiplier(currentAnimation->playbackSpeed);
        }
    }

    shaderToUpdate.setMat4Array(location, getTransforms());
}

std::vector<glm::mat4> SkeletalAnimationControllerComponent::getTransforms() {
    const auto currentAniamtionEntry = mName2animation.find(mCurrentAnimation);
    if (currentAniamtionEntry == mName2animation.end()) {
        return std::vector<glm::mat4>{ mSkeleton.size(), glm::mat4{ 1.f } };
    }
    const SkeletalAnimation& animation = currentAniamtionEntry->second;

    std::vector<glm::mat4> transforms = animation.getAt(getCurrentAnimationTime(), mSkeleton);
    for (std::size_t i = 0; i < transforms.size(); ++i) {
        auto& bone = mSkeleton[i];
        if (bone.parent != -1)
            transforms[i] = (transforms[bone.parent] * transforms[i]);  // parents are always before their children
                                                                        // so it is ok
    }

    for (std::size_t i = 0; i < transforms.size(); ++i)
        transforms[i] *= mSkeleton[i].offset;

    return transforms;
}

bool SkeletalAnimationControllerComponent::playCurrentAnimation() {
    const bool isPaused = isCurrentAnimationPaused();
    const bool isStopped = isCurrentAnimationStopped();
    if (isPaused) {
        mTime.resume();
    } else if (isStopped) {
        mTime.reset();
    }
    return isPaused || isStopped;
}

bool SkeletalAnimationControllerComponent::pauseCurrentAnimation() {
    const bool canBePaused = isCurrentAnimationPlaying();
    if (canBePaused) {
        mTime.pause();
    }

    return canBePaused;
}

bool SkeletalAnimationControllerComponent::stopCurrentAnimation() {
    const bool canBeStopped = isCurrentAnimationPlaying() || isCurrentAnimationPaused();
    if (canBeStopped) {
        mTime.stop();
    }

    return canBeStopped;
}

bool SkeletalAnimationControllerComponent::isCurrentAnimationPlaying() const {
    return getCurrentAnimation() != nullptr && !mTime.isPaused() && !isCurrentAnimationStopped();
}

bool SkeletalAnimationControllerComponent::isCurrentAnimationPaused() const {
    return getCurrentAnimation() != nullptr && !isCurrentAnimationStopped() && mTime.isPaused();
}

bool SkeletalAnimationControllerComponent::isCurrentAnimationStopped() const {
    const SkeletalAnimation* anim = getCurrentAnimation();
    return anim != nullptr
        && (!mTime.isStarted()
            || (anim->loopDirection == SkeletalAnimation::LoopDirection::STOP
                && mTime.getSeconds() > anim->getDuration()));
}

float SkeletalAnimationControllerComponent::getCurrentAnimationTime() const {
    const SkeletalAnimation* currentAnimation = getCurrentAnimation();
    if (currentAnimation == nullptr || isCurrentAnimationStopped()) {
        return 0.f;
    }

    float ticks = mTime.getSeconds();
    const auto loopDirection = currentAnimation->loopDirection;
    const auto duration = currentAnimation->getDuration();

    if (loopDirection == SkeletalAnimation::LoopDirection::REPEAT) {
        ticks = std::fmod(ticks, duration);
    }
    if (loopDirection == SkeletalAnimation::LoopDirection::BOUNCE) {
        ticks = (static_cast<int>(ticks / duration) % 2 == 0 ? std::fmod(ticks, duration)
                                                             : duration - std::fmod(ticks, duration));
    }

    return ticks;
}

void SkeletalAnimationControllerComponent::setCurrentAnimationTime(float time) {
    if (getCurrentAnimation() == nullptr) {
        return;
    }

    if (isCurrentAnimationStopped()) {
        mTime.reset();
        mTime.pause();
    }

    mTime.setSeconds(time);
}
