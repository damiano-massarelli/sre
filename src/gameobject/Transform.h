#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "GameObjectEH.h"
#include <vector>
#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

class Transform {
    friend class GameObjectEH;

    private:
        glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
        glm::quat mRotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 mScale{1.0f, 1.0f, 1.0f};

        GameObjectEH gameObject;
        GameObjectEH mParent;
        std::vector<GameObjectEH> mChildren;

    public:
        /**
          * Sets the world position of this transform
          * @param position the world position */
        void setPosition(const glm::vec3& position);

        /**
          * Moves the transform by a certain amount
          * @param amount how much is the transform moved */
        void moveBy(const glm::vec3& amount);

        /**
          * Sets the rotation in world space
          * @param rotation rotation in world space */
        void setRotation(const glm::quat& rotation);

        void setRotation(const glm::quat& rotation, const glm::vec3& pivot);

        /**
          * Rotates the transform by a certain amount
          * @param amount how much is the transform rotated */
        void rotateBy(const glm::quat& amount);

        void rotateBy(const glm::quat& amount, const glm::vec3& pivot);

        /**
          * Sets the scale in world space
          * @param scale the scale in world space */
        void setScale(const glm::vec3& scale);

        void setScale(const glm::vec3& scale, const glm::vec3& pivot);

        /**
          * Scales the transform by a certain amount
          * @param amount how much is the transform scaled */
        void scaleBy(const glm::vec3& amount);

        void scaleBy(const glm::vec3& amount, const glm::vec3& pivot);

        /**
          * @return world space position */
        const glm::vec3& getPosition() const;

        /**
          * @return world space rotation */
        const glm::quat& getRotation() const;

        /**
          * @return world space scale */
        const glm::vec3& getScale() const;

        /**
          * Computes and returns the model to world matrix
          * Use this matrix to transform vectors from model space to world space
          * @return the model to world matrix */
        glm::mat4 modelToWorld() const;

        /**
          * Computes and return the model to world matrix only taking into
          * account the rotation of this object
          * @return the rotation model to world matrix */
        glm::mat3 modelToUpright() const;

        /**
          * Y-axis direction of this model
          * Returns the direction of the Y-axis of this model in world space
          * @see modelToWorld
          * @return Y-axis direction of this model in world space */
        glm::vec3 up() const;

        /**
          * X-axis direction of this model
          * Returns the direction of the X-axis of this model in world space
          * @see modelToWorld
          * @return X-axis direction of this model in world space */
        glm::vec3 right() const;

        /**
          * Z-axis direction of this model
          * Returns the direction of the Z-axis of this model in world space
          * @see modelToWorld
          * @return Z-axis direction of this model in world space */
        glm::vec3 forward() const;

        /**
          * Adds a child to this transform's game object
          * @param child the child to add */
        void addChild(const GameObjectEH& child);

        /**
          * Removes a child
          * @param child the child to remove */
        void removeChild(const GameObjectEH& child);

        /**
          * Sets the parent of this transform's game object
          * @param parent the parent to set */
        void setParent(const GameObjectEH& parent);

        /**
          * Removes the parent. */
        void removeParent();

        /**
          * @return this transform's children */
        const std::vector<GameObjectEH>& getChildren();
};

#endif // TRANSFORM_H