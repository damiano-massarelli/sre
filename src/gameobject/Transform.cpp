#include "Transform.h"
#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

#include <iostream>

glm::mat4 Transform::modelToWorld() const
{
    glm::mat4 m2w = glm::translate(glm::mat4{1.0f}, mPosition);
    m2w *= glm::toMat4(mRotation);
    m2w = glm::scale(m2w, mScale);
    return m2w;
}

void Transform::setPosition(const glm::vec3& position)
{
    glm::vec3 diff = position - mPosition;
    mPosition = position;
    for (auto& child : mChildren)
        child->transform.moveBy(diff);
}

void Transform::moveBy(const glm::vec3& amount)
{
    setPosition(getPosition() + amount);
}

void Transform::setRotation(const glm::quat& rotation)
{
    setRotation(rotation, mPosition);
}

void Transform::setRotation(const glm::quat& rotation, const glm::vec3& pivot)
{
    glm::quat diff = glm::normalize(rotation * glm::conjugate(mRotation));

    mRotation = rotation;
    auto pos = mPosition - pivot;
    pos = glm::toMat3(diff) * pos;
    mPosition = pos + pivot;

    for (auto& child : mChildren)
        child->transform.rotateBy(diff, pivot);
}

void Transform::rotateBy(const glm::quat& amount)
{
    setRotation(amount * mRotation);
}

void Transform::rotateBy(const glm::quat& amount, const glm::vec3& pivot)
{
    setRotation(amount * mRotation, pivot);
}

void Transform::setScale(const glm::vec3& scale)
{
    setScale(scale, mPosition);
}

void Transform::setScale(const glm::vec3& scale, const glm::vec3& pivot)
{
    glm::vec3 diff{scale.x / mScale.x, scale.y / mScale.y, scale.z / mScale.z};
    auto pos = mPosition - pivot;
    pos = glm::mat3{glm::scale(glm::mat4{1.0f}, diff)} * pos;
    mPosition = pos + pivot;

    mScale = scale;

    for (auto& child : mChildren)
        child->transform.scaleBy(diff, pivot);
}

void Transform::scaleBy(const glm::vec3& amount)
{
    setScale(mScale * amount, mPosition);
}

void Transform::scaleBy(const glm::vec3& amount, const glm::vec3& pivot)
{
    setScale(mScale * amount, pivot);
}

const glm::vec3& Transform::getPosition() const
{
    return mPosition;
}

const glm::quat& Transform::getRotation() const
{
    return mRotation;
}

const glm::vec3& Transform::getScale() const
{
    return mScale;
}

glm::mat3 Transform::modelToUpright() const
{
    return glm::toMat3(mRotation);
}

glm::vec3 Transform::up() const
{
    return modelToUpright() * glm::vec3{0.0f, 1.0f, 0.0f};
}

glm::vec3 Transform::right() const
{
    return modelToUpright() * glm::vec3{1.0f, 0.0f, 0.0f};
}

glm::vec3 Transform::forward() const
{
    return modelToUpright() * glm::vec3{0.0f, 0.0f, 1.0f};
}

void Transform::setParent(const GameObjectEH& parent)
{
    if (mParent) // in case parent is not valid at least we remove its child
        mParent->transform.removeChild(gameObject);

    if (parent) // addChild will actually set the parent
        parent->transform.addChild(gameObject);
}

void Transform::addChild(const GameObjectEH& child)
{
    if (child->transform.mParent != gameObject) { // checks if not already father
        Transform& childTransform = child->transform;
        if (childTransform.mParent)
            childTransform.mParent->transform.removeChild(child);

        childTransform.mParent = gameObject;
        mChildren.push_back(child);
    }
}

void Transform::removeChild(const GameObjectEH& child)
{
    child->transform.mParent = GameObjectEH(); // invalid eh means no parent
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
}

void Transform::removeParent()
{
    if (mParent)
        mParent->transform.removeChild(gameObject);
}

const std::vector<GameObjectEH>& Transform::getChildren()
{
    return mChildren;
}

