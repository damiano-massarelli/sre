#include "gameobject/Transform.h"
#include "gameobject/GameObject.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <queue>

glm::mat4 Transform::modelToWorld() const {
    if (mModelWorldCacheValid)
        return mCacheModelToWorld;
    glm::mat4 m2w = glm::translate(glm::mat4{ 1.0f }, mPosition);
    m2w *= glm::toMat4(mRotation);
    m2w = glm::scale(m2w, mScale);
    return m2w;
}

glm::mat4 Transform::modelToWorld() {
    if (!mModelWorldCacheValid) {
        mCacheModelToWorld = glm::translate(glm::mat4{ 1.0f }, mPosition);
        mCacheModelToWorld *= glm::toMat4(mRotation);
        mCacheModelToWorld = glm::scale(mCacheModelToWorld, mScale);
        mModelWorldCacheValid = true;
    }
    return mCacheModelToWorld;
}

void Transform::askParentToUpdateBoundingBox() {
    if (mParent) {
        std::string name = gameObject->name;
        Transform& parentTransform = mParent->transform;
        parentTransform.mShouldUpdateChildrenBoundingBox = true;
        parentTransform.askParentToUpdateBoundingBox();  // recursive call
    }
}

void Transform::setPosition(const glm::vec3& position) {
    askParentToUpdateBoundingBox();
    mShouldUpdateTransformedBoundingBox = true;
    mModelWorldCacheValid = false;
    mModelWorldNormalCacheValid = false;
    glm::vec3 diff = position - mPosition;
    mPosition = position;
    for (auto& child : mChildren)
        child->transform.moveBy(diff);
}

void Transform::moveBy(const glm::vec3& amount) {
    setPosition(getPosition() + amount);
}

void Transform::setRotation(const glm::quat& rotation) {
    setRotation(rotation, mPosition);
}

void Transform::setRotation(const glm::quat& rotation, const glm::vec3& pivot) {
    askParentToUpdateBoundingBox();
    mShouldUpdateTransformedBoundingBox = true;
    mModelWorldCacheValid = false;
    mModelWorldNormalCacheValid = false;

    glm::quat diff = glm::normalize(rotation * glm::conjugate(mRotation));

    mRotation = rotation;
    auto pos = mPosition - pivot;
    pos = glm::toMat3(diff) * pos;
    mPosition = pos + pivot;

    for (auto& child : mChildren)
        child->transform.rotateBy(diff, pivot);
}

void Transform::rotateBy(const glm::quat& amount) {
    setRotation(amount * mRotation);
}

void Transform::rotateBy(const glm::quat& amount, const glm::vec3& pivot) {
    setRotation(amount * mRotation, pivot);
}

void Transform::setScale(const glm::vec3& scale) {
    setScale(scale, mPosition);
}

void Transform::setScale(const glm::vec3& scale, const glm::vec3& pivot) {
    askParentToUpdateBoundingBox();
    mShouldUpdateTransformedBoundingBox = true;
    mModelWorldCacheValid = false;
    mModelWorldNormalCacheValid = false;

    glm::vec3 diff{ scale.x / mScale.x, scale.y / mScale.y, scale.z / mScale.z };
    auto pos = mPosition - pivot;
    pos = glm::mat3{ glm::scale(glm::mat4{ 1.0f }, diff) } * pos;
    mPosition = pos + pivot;

    mScale = scale;

    for (auto& child : mChildren)
        child->transform.scaleBy(diff, pivot);
}

void Transform::scaleBy(const glm::vec3& amount) {
    setScale(mScale * amount, mPosition);
}

void Transform::scaleBy(const glm::vec3& amount, const glm::vec3& pivot) {
    setScale(mScale * amount, pivot);
}

BoundingBox Transform::getBoundingBox() {
    if (!mShouldUpdateMeshBoundingBox && !mShouldUpdateChildrenBoundingBox && !mShouldUpdateTransformedBoundingBox) {

        return mCachedBoundingBox;
    }

    /* if the number of meshes changed we need to update the bb */
    if (mShouldUpdateMeshBoundingBox) {
        mCachedMeshesBoundingBox = BoundingBox();
        for (const Mesh& mesh : gameObject->getMeshes()) {
            mCachedMeshesBoundingBox.extend(mesh.boundingBox);
        }

        mShouldUpdateMeshBoundingBox = false;

        // also update the transformed bb
        mShouldUpdateTransformedBoundingBox = true;
    }

    if (mShouldUpdateTransformedBoundingBox) {
        mCachedTransformedBoundingBox = mCachedMeshesBoundingBox.transformed(modelToWorld());
        mShouldUpdateTransformedBoundingBox = false;
    }

    if (mShouldUpdateChildrenBoundingBox) {
        mCachedChildrenBoundingBox = BoundingBox();
        for (const GameObjectEH& go : getChildren()) {
            mCachedChildrenBoundingBox.extend(go->transform.getBoundingBox());
        }
        mShouldUpdateChildrenBoundingBox = false;
    }

    mCachedBoundingBox = BoundingBox();
    mCachedBoundingBox.extend(mCachedTransformedBoundingBox);
    mCachedBoundingBox.extend(mCachedChildrenBoundingBox);

    return mCachedBoundingBox;
}

void Transform::updateMeshBoundingBox() {
    mShouldUpdateMeshBoundingBox = true;
    askParentToUpdateBoundingBox();
}

void Transform::lookAt(const glm::vec3& position) {
    glm::vec3 dir = glm::normalize(position - getPosition());

    // this engine uses a right-handed system
    // however, the look vector is +z (typical of LH systems)
    auto rot = glm::quatLookAtLH(dir, glm::vec3{ 0.F, 1.F, 0.F });
    setRotation(glm::normalize(rot));
}

const glm::vec3& Transform::getPosition() const {
    return mPosition;
}

const glm::quat& Transform::getRotation() const {
    return mRotation;
}

const glm::vec3& Transform::getScale() const {
    return mScale;
}

void Transform::setLocalPosition(const glm::vec3& localPosition) {
    glm::vec3 parentPos{ 0.0f };
    if (mParent)
        parentPos = mParent->transform.getPosition();
    setPosition(parentPos + localPosition);
}

void Transform::setLocalRotation(const glm::quat& localRotation) {
    glm::quat parentRot{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 parentPos{ 0.0f };
    if (mParent) {
        Transform& transform = mParent->transform;
        parentRot = transform.getRotation();
        parentPos = transform.getPosition();
    }
    setRotation(parentRot * localRotation, parentPos);
}

void Transform::setLocalScale(const glm::vec3 localScale) {
    glm::vec3 parentScale{ 1.0f, 1.0f, 1.0f };
    glm::vec3 parentPos{ 0.0f };
    if (mParent) {
        Transform& transform = mParent->transform;
        parentScale = transform.getScale();
        parentPos = transform.getPosition();
    }
    setScale(parentScale * localScale, parentPos);
}

glm::vec3 Transform::getLocalPosition() const {
    glm::vec3 parentPos{ 0.0f };
    if (mParent)
        parentPos = mParent->transform.getPosition();
    return getPosition() - parentPos;
}

glm::quat Transform::getLocalRotation() const {
    glm::quat parentRot{ 1.0f, 0.0f, 0.0f, 0.0f };
    if (mParent)
        parentRot = mParent->transform.getRotation();
    return getRotation() * glm::conjugate(parentRot);
}

glm::vec3 Transform::getLocalScale() const {
    glm::vec3 parentScale{ 1.0f, 1.0f, 1.0f };
    if (mParent)
        parentScale = mParent->transform.getScale();
    auto scale = getScale();
    return glm::vec3{ scale.x / parentScale.x, scale.y / parentScale.y, scale.z / parentScale.z };
}

glm::mat3 Transform::modelToWorldForNormals() const {
    if (mModelWorldNormalCacheValid)
        return mCacheModelToWorldNormal;
    return glm::inverse(glm::transpose(glm::mat3(modelToWorld())));
}

glm::mat3 Transform::modelToWorldForNormals() {
    if (!mModelWorldNormalCacheValid) {
        mCacheModelToWorldNormal = glm::inverse(glm::transpose(glm::mat3(modelToWorld())));
        mModelWorldNormalCacheValid = true;
    }
    return mCacheModelToWorldNormal;
}

glm::mat3 Transform::modelToUpright() const {
    return glm::toMat3(mRotation);
}

glm::vec3 Transform::up() const {
    return modelToUpright() * glm::vec3{ 0.0f, 1.0f, 0.0f };
}

glm::vec3 Transform::right() const {
    return modelToUpright() * glm::vec3{ 1.0f, 0.0f, 0.0f };
}

glm::vec3 Transform::forward() const {
    return modelToUpright() * glm::vec3{ 0.0f, 0.0f, 1.0f };
}

void Transform::setParent(const GameObjectEH& parent) {
    if (mParent)  // in case parent is not valid at least we remove its child
        mParent->transform.removeChild(gameObject);

    if (parent)  // addChild will actually set the parent
        parent->transform.addChild(gameObject);
}

void Transform::addChild(const GameObjectEH& child) {
    if (child->transform.mParent != gameObject) {  // checks if not already father
        Transform& childTransform = child->transform;
        if (childTransform.mParent)
            childTransform.mParent->transform.removeChild(child);

        childTransform.mParent = gameObject;
        mChildren.push_back(child);

        // new child bb should be updated
        askParentToUpdateBoundingBox();
        mShouldUpdateChildrenBoundingBox = true;
    }
}

void Transform::removeChild(const GameObjectEH& child) {
    child->transform.mParent = GameObjectEH();  // invalid eh means no parent
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());

    askParentToUpdateBoundingBox();
    mShouldUpdateChildrenBoundingBox = true;
}

void Transform::removeParent() {
    if (mParent)
        mParent->transform.removeChild(gameObject);
}

const std::vector<GameObjectEH>& Transform::getChildren() const {
    return mChildren;
}

std::vector<GameObjectEH> Transform::findAll(const std::string& name) const {
    std::vector<GameObjectEH> found;

    std::queue<GameObjectEH> searching;
    searching.push(gameObject);

    while (!searching.empty()) {
        auto eh = searching.front();
        searching.pop();

        if (eh->name == name)
            found.push_back(eh);

        for (auto child : eh->transform.getChildren())
            searching.push(child);
    }

    return found;
}

GameObjectEH Transform::find(const std::filesystem::path& path) const {
    return find(path.begin(), path.end());
}

GameObjectEH Transform::find(std::filesystem::path::iterator it, std::filesystem::path::iterator end) const {
    if (it == end)
        return gameObject;
    else if (*it == "..")
        return mParent->transform.find(++it, end);
    else if (*it == ".")
        return find(++it, end);

    for (const auto& child : getChildren())
        if (child->name == *it)
            return child->transform.find(++it, end);

    return GameObjectEH{};
}
