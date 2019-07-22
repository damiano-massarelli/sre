#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "GameObjectEH.h"
#include <vector>
#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include <filesystem>

class Transform {
friend struct GameObjectEH;

private:
    glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
    glm::quat mRotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 mScale{1.0f, 1.0f, 1.0f};

	bool mModelWorldCacheValid = false;
	glm::mat4 mCacheModelToWorld;

	bool mModelWorldNormalCacheValid = false;
	glm::mat4 mCacheModelToWorldNormal;

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

    /**
        * Sets the rotation in world space
        * @param rotation rotation in world space
        * @param pivot the center of rotation */
    void setRotation(const glm::quat& rotation, const glm::vec3& pivot);

    /**
        * Rotates the transform by a certain amount
        * @param amount how much is the transform rotated */
    void rotateBy(const glm::quat& amount);

    /**
        * Rotates the transform by a certain amount around a pivot point.
        * @param amount how much is the transform rotated
        * @param pivot the center of rotation */
    void rotateBy(const glm::quat& amount, const glm::vec3& pivot);

    /**
        * Sets the scale in world space.
        * @param scale the scale in world space */
    void setScale(const glm::vec3& scale);

    /**
        * Sets the scale in world space.
        * @param scale the scale in world space
        * @param pivot the center of scaling */
    void setScale(const glm::vec3& scale, const glm::vec3& pivot);

    /**
        * Scales the transform by a certain amount.
        * @param amount how much is the transform scaled */
    void scaleBy(const glm::vec3& amount);

    /**
        * Scales the transform by a certain amount.
        * @param amount how much is the transform scaled
        * @param pivot the center of scaling */
    void scaleBy(const glm::vec3& amount, const glm::vec3& pivot);

	/**
		* Rotates this transform by a certain amount.
		* @param position the position this transform will be looking at 
		*/
	void lookAt(const glm::vec3& position);

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
        * Sets the position of this Transform relative to its parent.
        * If this Transform does not have a parent this is equivalent to setPosition()
        * @param localPosition the position relative to the parent
        * @sa setParent() */
    void setLocalPosition(const glm::vec3& localPosition);

    /**
        * Sets the rotation of this Transform relative to its parent.
        * If this Transform does not have a parent this is equivalent to setRotation()
        * @param localRotation the rotation relative to the parent
        * @sa setParent() */
    void setLocalRotation(const glm::quat& localRotation);

    /**
        * Sets the scale of this Transform relative to its parent.
        * If this Transform does not have a parent this is equivalent to setScale()
        * @param localScale the scale relative to the parent
        * @sa setParent() */
    void setLocalScale(const glm::vec3 localScale);

    /** @return position relative to parent */
    glm::vec3 getLocalPosition() const;

    /** @return rotation relative to parent */
    glm::quat getLocalRotation() const;

    /** @return scale relative to parent */
    glm::vec3 getLocalScale() const;

    /**
        * Computes and returns the model to world matrix.
        * Use this matrix to transform vectors from model space to world space.
		* The non const version of this method caches the result.
        * @return the model to world matrix */
    glm::mat4 modelToWorld() const;

	/**
	  * Computes and returns the model to world matrix.
	  * Use this matrix to transform vectors from model space to world space.
	  * The non const version of this method caches the result.
	  * @return the model to world matrix */
	glm::mat4 modelToWorld();

	/**
	 * Computes and return the model to world matrix used for normals.
	 * The non const version of this method caches the result.
	 * @return the model to world matrix used for normals.
	 */
	glm::mat3 modelToWorldForNormals() const;

	/**
     * Computes and return the model to world matrix used for normals.
     * The non const version of this method caches the result.
     * @return the model to world matrix used for normals.
     */
	glm::mat3 modelToWorldForNormals();

    /**
		* Computes and return the model to world matrix only taking into.
        * account the rotation of this object
        * @return the rotation model to world matrix */
    glm::mat3 modelToUpright() const;

    /**
        * Y-axis direction of this model.
        * Returns the direction of the Y-axis of this model in world space
        * @see modelToWorld
        * @return Y-axis direction of this model in world space */
    glm::vec3 up() const;

    /**
        * X-axis direction of this model.
        * Returns the direction of the X-axis of this model in world space
        * @see modelToWorld
        * @return X-axis direction of this model in world space */
    glm::vec3 right() const;

    /**
        * Z-axis direction of this model.
        * Returns the direction of the Z-axis of this model in world space
        * @see modelToWorld
        * @return Z-axis direction of this model in world space */
    glm::vec3 forward() const;

    /**
        * Adds a child to this Transform%'s GameObject.
        * @param child the child to add */
    void addChild(const GameObjectEH& child);

    /**
        * Removes a child.
        * @param child the child to remove */
    void removeChild(const GameObjectEH& child);

    /**
        * Sets the parent of this Transform%'s GameObject.
        * @param parent the parent to set */
    void setParent(const GameObjectEH& parent);

    /**
      * Removes the parent. */
    void removeParent();

    /**
      * @return this Transform%'s children */
    const std::vector<GameObjectEH>& getChildren();

	std::vector<GameObjectEH> findAll(const std::string& name);

	GameObjectEH find(const std::filesystem::path& path);

	GameObjectEH find(const std::filesystem::path::iterator it);
};

#endif // TRANSFORM_H
