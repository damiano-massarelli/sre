#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <vector>

struct BoundingBox {
private:
    glm::vec3 mMin{ std::numeric_limits<float>::infinity() };
    glm::vec3 mMax{ -std::numeric_limits<float>::infinity() };

    void updateMinMax(const glm::vec3& newPoint);

public:
    /**
     * Creates a infinite bb.
     */
    BoundingBox();

    /**
     * Creates a new bounding box for the given points.
     * @param points a vector of points that will be contained in the bounding
     * box.
     */
    BoundingBox(const std::vector<glm::vec3>& points);

    /**
     * Creates a new bounding box given two points representing its extent.
     * @param minPoint the point with the min values for x, y and z
     * @param maxPoint the point with the min values for x, y and z
     */
    BoundingBox(const glm::vec3& minPoint, const glm::vec3& maxPoint);

    /**
     * @return the min vertex of this bb
     */
    const glm::vec3& getMin() const;

    /**
     * @return the max vertex of this bb
     */
    const glm::vec3& getMax() const;

    /**
     * @return the extent of this bb
     */
    const glm::vec3 getExtent() const;

    /**
     * @return the positive diagonal of the bb
     */
    const glm::vec3 getDiagonal() const;

    /**
     * @return the center of this bb
     */
    const glm::vec3 getCenter() const;

    /**
     * Extends this bb.
     * This bb is updated taking into account the new point.
     * @param newPoint this bb will be extended to contain this point (may not
     * be necessary)
     */
    void extend(const glm::vec3& newPoint);

    /**
     * Extends this bb.
     * The resulting BoundingBox is the BoundingBox of the union of boundingBox
     * and this bb.
     * @param boundingBox the current bb will be extended to contain this bb.
     */
    void extend(const BoundingBox& boundingBox);

    /**
     * A valid bb is one for which it makes sense to get the center or the
     * extent. To be valid, every operation performed on the vertices of the bb
     * must result in a value different from nan.
     * @return whether this bb is valid
     */
    bool isValid() const;

    /**
     * Creates and returns a transformed version of this bb.
     * @param transformationMatrix the matrix representing the transformation
     * @return the transformed version of this bb.
     */
    BoundingBox transformed(const glm::mat4& transformationMatrix) const;
};

std::ostream& operator<<(std::ostream& out, const BoundingBox& bb);