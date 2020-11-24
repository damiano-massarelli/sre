#include "CameraComponent.h"
#include "Engine.h"

void CameraComponent::calculateFrustumVertices() {
    constexpr glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
    constexpr glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 left{ 1.0f, 0.0f, 0.0f };

    float aspectRatio = Engine::renderSys.getScreenWidth() / (float)Engine::renderSys.getScreenHeight();

    float halfFovY = mFov / 2;
    float halfNearHeight = std::tan(halfFovY) * mNearPlaneDistance;
    float halfFarHeight = std::tan(halfFovY) * mFarPlaneDistance;

    float halfFarWidth = halfFarHeight * aspectRatio;
    float halfNearWidth = halfNearHeight * aspectRatio;

    glm::vec3 centerNear = forward * mNearPlaneDistance;
    glm::vec3 centerFar = forward * mFarPlaneDistance;

    glm::vec3 toFarTopLeft = up * halfFarHeight + left * halfFarWidth;
    glm::vec3 toFarBottomRight = -toFarTopLeft;
    glm::vec3 toFarTopRight = up * halfFarHeight - left * halfFarWidth;
    glm::vec3 toFarBottomLeft = -toFarTopRight;

    glm::vec3 toNearTopLeft = up * halfNearHeight + left * halfNearWidth;
    glm::vec3 toNearBottomRight = -toNearTopLeft;
    glm::vec3 toNearTopRight = up * halfNearHeight - left * halfNearWidth;
    glm::vec3 toNearBottomLeft = -toNearTopRight;

    mFrustumVertices = std::array<glm::vec3, 8>{ centerFar + toFarTopRight,
        centerFar + toFarBottomLeft,
        centerFar + toFarTopLeft,
        centerFar + toFarBottomRight,

        centerNear + toNearTopRight,
        centerNear + toNearBottomLeft,
        centerNear + toNearTopLeft,
        centerNear + toNearBottomRight };
}

CameraComponent::CameraComponent(const GameObjectEH& go, float fov, float nearPlaneDistance, float farPlaneDistance)
    : Component{ go }
    , mFov{ fov }
    , mNearPlaneDistance{ nearPlaneDistance }
    , mFarPlaneDistance{ farPlaneDistance } {
    calculateFrustumVertices();
}

void CameraComponent::setFOV(float fov) {
    mFov = fov;

    calculateFrustumVertices();

    // updates the camera
    if (gameObject == Engine::renderSys.getCamera()) {
        Engine::renderSys.setCamera(gameObject);
    }
}

float CameraComponent::getFOV() const {
    return mFov;
}

float CameraComponent::getNearPlaneDistance() const {
    return mNearPlaneDistance;
}

float CameraComponent::getFarPlaneDistance() const {
    return mFarPlaneDistance;
}

Frustum CameraComponent::getViewFrutsum() const {
    std::array<glm::vec3, 8> worldSpaceVertices;
    glm::mat4 cameraTransform = gameObject->transform.modelToWorld();
    std::transform(mFrustumVertices.begin(),
        mFrustumVertices.end(),
        worldSpaceVertices.begin(),
        [&cameraTransform](const glm::vec3& vertex) {
            return glm::vec3{ cameraTransform * glm::vec4{ vertex, 1.0f } };
        });

    return Frustum(worldSpaceVertices);
}
