#ifndef MESH_H
#define MESH_H
#include "geometry/BoundingBox.h"
#include "resourceManagment/RefCount.h"
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

/**
 * The basic information about the vertex of a Mesh
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

/**
 * A Mesh contains all the data needed to render an objects (vao, vbo, etc)
 */
class Mesh {
    // Render system should be able to access this class data
    friend class GameObjectRenderer;
    friend class MeshLoader;
    friend class RenderSystem;

public:
    RefCount refCount;

    /**
     * This mesh's bounding box
     */
    BoundingBox boundingBox;

private:
    GLuint mVao = 0;
    std::vector<GLuint> mBuffers;

    // useful to change the LOD of a mesh
    GLuint mEbo = 0;

    bool mUsesIndices = false;

    GLenum mDrawMode = GL_TRIANGLES;

    /// whether or not this class stores information about its vertices
    bool mHasVertexData = false;
    std::vector<Vertex> mVertexData;

    std::uint32_t mVertexNumber = 0;
    std::uint32_t mIndicesNumber = 0;

    Mesh(std::uint32_t vao);

    /**
     * Frees the memory held by this mesh */
    void cleanUpIfNeeded();

public:
    Mesh() = default;

    /**
     * @return the VAO used to render this mesh.
     */
    GLuint getVao() const;

    /**
     * @return the EBO used to index the vertices of this mesh.
     */
    GLuint getEbo() const;

    /**
     * Returns vertex data for this mesh.
     * In case vertex data is not stored in this mesh an empty vector is
     * returned and hasVertexData should return false
     * @sa GameObjectLoader::keepVertexData
     * @return a vector of Vertex containing data for each vertex */
    const std::vector<Vertex>& getVertexData() const;

    /**
     * Checks whether this mesh has vertex data.
     * @return true if vertex data is stored in this mesh, false otherwise. */
    bool hasVertexData() const;

    Mesh& operator=(const Mesh& rhs);

    virtual ~Mesh();
};

#endif  // MESH_H
