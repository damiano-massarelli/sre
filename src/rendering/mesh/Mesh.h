#ifndef MESH_H
#define MESH_H
#include <cstdint>
#include <glm/glm.hpp>
#include <glad/glad.h>
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
class Mesh
{
    // Render system should be able to access this class data
    friend class RenderSystem;
    friend class MeshLoader;

    private:
        std::uint32_t mVao = 0;
        std::uint32_t mVbo = 0;
        std::uint32_t mEbo = 0;

        bool usesIndices = false;

        int drawMode = GL_TRIANGLES;

        /// whether or not this class stores information about its vertices
        bool mHasVertexData = false;
        std::vector<Vertex> mVertexData;

        std::uint32_t mVertexNumber = 0;
        std::uint32_t mIndicesNumber = 0;

        Mesh(std::uint32_t vao, std::uint32_t vbo, std::uint32_t ebo);

    public:
        Mesh() = default;

        const std::vector<Vertex>& getVertexData() const;

        bool hasVertexData() const;

        void cleanUp();
};

#endif // MESH_H
